#pragma once

#include <cstddef>

#include "t81/vm/state.hpp"

namespace t81::vm::internal {

enum class TensorAllocPolicyResult {
  Allow = 0,
  MaxTensorsExceeded,
  MaxTensorElementsExceeded,
};

std::size_t tensor_shape_complexity(const t81::T729DynamicTensor& tensor);

TensorAllocPolicyResult evaluate_tensor_alloc_policy(const State& state, std::size_t tensor_elements);

std::size_t store_tensor_slot(State& state, t81::T729DynamicTensor tensor);

void account_tensor_allocation(State& state, std::size_t tensor_elements);

}  // namespace t81::vm::internal
