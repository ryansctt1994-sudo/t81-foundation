#include "test_runtime_check.hpp"

#include "core/vm/internal/tensor_helpers.hpp"
#include "t81/tensor.hpp"

int main() {
  using t81::T729DynamicTensor;
  using t81::vm::internal::tensor_elementwise_compatible;
  using t81::vm::internal::tensor_matmul_compatible;
  using t81::vm::internal::tensor_rmsnorm_compatible;
  using t81::vm::internal::tensor_rope_compatible;
  using t81::vm::internal::tensor_softmax_compatible;
  using t81::vm::internal::tensor_transpose_2d_compatible;

  const T729DynamicTensor vec3({3}, {1.0f, 2.0f, 3.0f});
  const T729DynamicTensor vec2({2}, {4.0f, 5.0f});
  const T729DynamicTensor mat23({2, 3}, {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f});
  const T729DynamicTensor mat32({3, 2}, {1.0f, 0.0f, 0.0f, 1.0f, 2.0f, 3.0f});
  const T729DynamicTensor mat22({2, 2}, {1.0f, 2.0f, 3.0f, 4.0f});
  const T729DynamicTensor scalar({}, {});

  T81_TEST_CHECK(tensor_elementwise_compatible(vec3, vec3));
  T81_TEST_CHECK(!tensor_elementwise_compatible(vec3, vec2));

  T81_TEST_CHECK(tensor_matmul_compatible(mat23, mat32));
  T81_TEST_CHECK(!tensor_matmul_compatible(mat23, mat22));

  T81_TEST_CHECK(tensor_rmsnorm_compatible(mat23, vec3));
  T81_TEST_CHECK(!tensor_rmsnorm_compatible(mat23, vec2));
  T81_TEST_CHECK(!tensor_rmsnorm_compatible(vec3, mat22));

  T81_TEST_CHECK(tensor_rope_compatible(mat22));
  T81_TEST_CHECK(!tensor_rope_compatible(vec3));

  T81_TEST_CHECK(tensor_softmax_compatible(vec3));
  T81_TEST_CHECK(!tensor_softmax_compatible(scalar));

  T81_TEST_CHECK(tensor_transpose_2d_compatible(mat22));
  T81_TEST_CHECK(!tensor_transpose_2d_compatible(vec3));

  return 0;
}
