#include "internal/tensor_helpers.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "t81/tracing/canonhash.hpp"

namespace t81::vm::internal {

std::size_t tensor_shape_complexity(const t81::T729DynamicTensor& tensor) {
  std::size_t product = 1;
  for (int dim : tensor.shape()) {
    if (dim <= 0) {
      return 0;
    }
    product *= static_cast<std::size_t>(dim);
  }
  return product * static_cast<std::size_t>(tensor.rank());
}

TensorAllocPolicyResult evaluate_tensor_alloc_policy(const State& state, std::size_t tensor_elements) {
  if (!state.policy) {
    return TensorAllocPolicyResult::Allow;
  }

  const std::size_t active_tensors = state.tensors.size() - state.free_tensor_indices.size();
  if (state.policy->max_tensors &&
      active_tensors >= static_cast<std::size_t>(*state.policy->max_tensors)) {
    return TensorAllocPolicyResult::MaxTensorsExceeded;
  }
  if (state.policy->max_tensor_elements &&
      state.total_tensor_elements + tensor_elements >
          static_cast<std::size_t>(*state.policy->max_tensor_elements)) {
    return TensorAllocPolicyResult::MaxTensorElementsExceeded;
  }
  return TensorAllocPolicyResult::Allow;
}

std::size_t store_tensor_slot(State& state, t81::T729DynamicTensor tensor) {
  std::size_t idx_handle = 0;
  if (!state.free_tensor_indices.empty()) {
    const auto raw_idx = state.free_tensor_indices.back();
    state.free_tensor_indices.pop_back();
    state.tensors[raw_idx] = std::move(tensor);
    idx_handle = raw_idx + 1;
  } else {
    state.tensors.push_back(std::move(tensor));
    idx_handle = state.tensors.size();
  }
  return idx_handle;
}

void account_tensor_allocation(State& state, std::size_t tensor_elements) {
  state.total_tensor_elements += tensor_elements;
  state.metrics.total_tensors++;
  state.metrics.total_tensor_elements += tensor_elements;
}

std::optional<t81::T729DynamicTensor> decode_native_tensor(const t81::weights::NativeTensor& native,
                                                           TensorDecodeMode mode) {
  std::vector<float> float_data;
  float_data.reserve(native.num_trits());

  if (native.format == t81::weights::NativeFormat::T3_K) {
    const uint8_t* byte_ptr = reinterpret_cast<const uint8_t*>(native.data.data());
    const uint64_t total_trits = native.num_trits();
    for (uint64_t offset = 0; offset < total_trits; offset += 128) {
      float scale;
      std::memcpy(&scale, byte_ptr, sizeof(float));
      byte_ptr += sizeof(float);
      const uint64_t count = std::min<uint64_t>(128, total_trits - offset);
      uint64_t trit_index = 0;
      for (uint64_t packed_idx = 0; packed_idx < 26; ++packed_idx) {
        uint8_t packed = *byte_ptr++;
        if (mode == TensorDecodeMode::StrictCanonical && packed > 242) {
          return std::nullopt;
        }
        uint8_t rem = packed;
        for (uint64_t local = 0; local < 5; ++local, ++trit_index) {
          const uint8_t digit = static_cast<uint8_t>(rem % 3);
          rem = static_cast<uint8_t>(rem / 3);
          if (trit_index < count) {
            const float trit = static_cast<float>(static_cast<int>(digit) - 1);
            float_data.push_back(trit * scale);
          } else if (mode == TensorDecodeMode::StrictCanonical && digit != 1) {
            // Canonical padding requires extra trits to be zero (mapped digit=1).
            return std::nullopt;
          }
        }
      }
    }
  } else {
    uint64_t remaining = native.trits;
    if (remaining == 0 && !native.data.empty()) {
      remaining = native.data.size() * 48;
    }
    for (uint64_t limb : native.data) {
      const uint64_t count = std::min<uint64_t>(48, remaining);
      std::vector<float> block(count);
      uint64_t val = limb;
      for (int i = 47; i >= 0; --i) {
        const uint64_t digit = val % 3;
        val /= 3;
        if (static_cast<uint64_t>(i) < count) {
          block[i] = static_cast<float>(static_cast<int>(digit) - 1);
        }
      }
      float_data.insert(float_data.end(), block.begin(), block.end());
      remaining -= count;
      if (remaining == 0) {
        break;
      }
    }
  }

  std::vector<int> shape;
  shape.reserve(native.shape.size());
  for (auto dim : native.shape) {
    shape.push_back(static_cast<int>(dim));
  }
  return t81::T729DynamicTensor(std::move(shape), std::move(float_data));
}

std::optional<t81::weights::NativeTensor> parse_canon_tensor_object(
    const std::vector<std::byte>& bytes) {
  // Header is 72 bytes: type(1), version(1), format(1), rank(1), reserved(4), shape(64).
  if (bytes.size() < 72) {
    return std::nullopt;
  }

  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(bytes.data());
  if (*ptr++ != 0x20) {
    return std::nullopt;
  }
  if (*ptr++ != 1) {
    return std::nullopt;
  }

  const uint8_t fmt = *ptr++;
  if (fmt != static_cast<uint8_t>(t81::weights::NativeFormat::BalancedTernary) &&
      fmt != static_cast<uint8_t>(t81::weights::NativeFormat::T3_K)) {
    return std::nullopt;
  }

  const uint8_t rank = *ptr++;
  if (rank > 8) {
    return std::nullopt;
  }
  ptr += 4;  // reserved

  t81::weights::NativeTensor native;
  native.format = static_cast<t81::weights::NativeFormat>(fmt);
  native.shape.reserve(rank);
  for (int i = 0; i < 8; ++i) {
    uint64_t dim = 0;
    for (int b = 0; b < 8; ++b) {
      dim |= (static_cast<uint64_t>(*ptr++) << (b * 8));
    }
    if (i < rank) {
      native.shape.push_back(dim);
    }
  }

  const size_t payload_bytes = bytes.size() - (ptr - reinterpret_cast<const uint8_t*>(bytes.data()));
  if (payload_bytes % 8 != 0) {
    return std::nullopt;
  }

  const size_t limbs = payload_bytes / 8;
  native.data.reserve(limbs);
  for (size_t i = 0; i < limbs; ++i) {
    uint64_t val = 0;
    for (int b = 0; b < 8; ++b) {
      val |= (static_cast<uint64_t>(*ptr++) << (b * 8));
    }
    native.data.push_back(val);
  }

  uint64_t trits = 1;
  for (uint64_t dim : native.shape) {
    trits *= dim;
  }
  native.trits = trits;
  return native;
}

std::optional<t81::T729DynamicTensor> decode_canon_tensor_object(
    const std::vector<std::byte>& bytes) {
  auto native = parse_canon_tensor_object(bytes);
  if (!native.has_value()) {
    return std::nullopt;
  }

  // CanonFS tensor objects are expected to carry packed payloads. If payload limbs equal
  // expanded element count, treat this as an ambiguous layout and fail closed.
  size_t payload_limbs = native->data.size();
  size_t expected_elements = 1;
  for (auto dim : native->shape) {
    expected_elements *= static_cast<size_t>(dim);
  }
  if (payload_limbs == expected_elements) {
    return std::nullopt;
  }

  return decode_native_tensor(*native, TensorDecodeMode::Lenient);
}

std::optional<t81::canonfs::CanonRef> parse_canon_tensor_ref(std::string_view hash_text) {
  std::string stripped(hash_text);
  if (stripped.rfind("sha3-256:", 0) == 0) {
    stripped = stripped.substr(9);
  }

  t81::canonfs::CanonHash ch;
  try {
    ch.h = t81::hash::CanonHash81::from_string(stripped);
  } catch (...) {
    return std::nullopt;
  }
  return t81::canonfs::CanonRef{ch};
}

}  // namespace t81::vm::internal
