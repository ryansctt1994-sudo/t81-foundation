#pragma once

#include <cstddef>
#include <optional>
#include <string_view>
#include <vector>

#include "t81/canonfs/canon_driver.hpp"
#include "t81/canonfs/canon_types.hpp"
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

std::optional<t81::T729DynamicTensor> decode_canon_tensor_object(
    const std::vector<std::byte>& bytes);

std::optional<t81::canonfs::CanonRef> parse_canon_tensor_ref(std::string_view hash_text);

enum class TensorLoadHashStatus {
  Ok = 0,
  InvalidHash,
  CanonFsMiss,
  DecodeFault,
};

struct TensorLoadHashResult {
  TensorLoadHashStatus status{TensorLoadHashStatus::DecodeFault};
  std::optional<t81::T729DynamicTensor> tensor;
};

TensorLoadHashResult load_canon_tensor_by_hash(t81::canonfs::Driver& driver,
                                               std::string_view hash_text);

t81::T729DynamicTensor tensor_unary_exp(const t81::T729DynamicTensor& tensor);
t81::T729DynamicTensor tensor_unary_silu(const t81::T729DynamicTensor& tensor);
t81::T729DynamicTensor tensor_unary_softmax(const t81::T729DynamicTensor& tensor);
bool tensor_elementwise_compatible(const t81::T729DynamicTensor& lhs,
                                   const t81::T729DynamicTensor& rhs);
bool tensor_softmax_compatible(const t81::T729DynamicTensor& tensor);
t81::T729DynamicTensor tensor_matmul_2d(const t81::T729DynamicTensor& lhs,
                                        const t81::T729DynamicTensor& rhs);
bool tensor_matmul_compatible(const t81::T729DynamicTensor& lhs,
                              const t81::T729DynamicTensor& rhs);
t81::T729DynamicTensor tensor_binary_elementwise(const t81::T729DynamicTensor& lhs,
                                                 const t81::T729DynamicTensor& rhs,
                                                 bool multiply);
bool tensor_transpose_2d_compatible(const t81::T729DynamicTensor& tensor);
t81::T729DynamicTensor tensor_transpose_2d(const t81::T729DynamicTensor& tensor);
std::optional<t81::T729DynamicTensor> tensor_contract_dot(const t81::T729DynamicTensor& lhs,
                                                          const t81::T729DynamicTensor& rhs);
t81::T729DynamicTensor tensor_rmsnorm(const t81::T729DynamicTensor& tensor,
                                      const t81::T729DynamicTensor& weights);
bool tensor_rmsnorm_compatible(const t81::T729DynamicTensor& tensor,
                               const t81::T729DynamicTensor& weights);
t81::T729DynamicTensor tensor_rope(const t81::T729DynamicTensor& tensor, int pos);
bool tensor_rope_compatible(const t81::T729DynamicTensor& tensor);

}  // namespace t81::vm::internal
