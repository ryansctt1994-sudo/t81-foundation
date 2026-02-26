#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "t81/weights.hpp"
#include "t81/vm/state.hpp"

namespace t81::vm::internal {

enum class TensorAllocPolicyResult {
  Allow = 0,
  MaxTensorsExceeded,
  MaxTensorElementsExceeded,
};

enum class TensorDecodeMode {
  StrictCanonical = 0,
  Lenient,
};

std::size_t tensor_shape_complexity(const t81::T729DynamicTensor& tensor);

TensorAllocPolicyResult evaluate_tensor_alloc_policy(const State& state, std::size_t tensor_elements);

std::size_t store_tensor_slot(State& state, t81::T729DynamicTensor tensor);

void account_tensor_allocation(State& state, std::size_t tensor_elements);

std::optional<t81::T729DynamicTensor> decode_native_tensor(
    const t81::weights::NativeTensor& native, TensorDecodeMode mode);

std::optional<t81::weights::NativeTensor> parse_canon_tensor_object(
    const std::vector<std::byte>& bytes);

}  // namespace t81::vm::internal
