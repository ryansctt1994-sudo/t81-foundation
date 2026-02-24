#include <cassert>
#include <iostream>
#include "t81/types/T81Tensor.hpp"

using namespace t81;

int main() {
  // 2x3 Tensor
  // [0 1 2]
  // [3 4 5]
  using Tensor2x3 = T81Tensor<T81Float<72, 9>, 2, 2, 3>;
  Tensor2x3 t;
  for (size_t i = 0; i < 6; ++i) t.data[i] = T81Float<72, 9>(static_cast<double>(i));

  // Permute (1, 0) -> Transpose -> 3x2
  // [0 3]
  // [1 4]
  // [2 5]
  auto p = permute<1, 0>(t);

  static_assert(decltype(p)::rank() == 2);
  static_assert(decltype(p)::shape()[0] == 3);
  static_assert(decltype(p)::shape()[1] == 2);

  assert(p(0, 0).to_double() == 0.0);
  assert(p(0, 1).to_double() == 3.0);
  assert(p(1, 0).to_double() == 1.0);
  assert(p(1, 1).to_double() == 4.0);
  assert(p(2, 0).to_double() == 2.0);
  assert(p(2, 1).to_double() == 5.0);

  // 3D Permute
  // 2x2x2
  using Tensor2x2x2 = T81Tensor<T81Float<72, 9>, 3, 2, 2, 2>;
  Tensor2x2x2 t3;
  for (size_t i = 0; i < 8; ++i) t3.data[i] = T81Float<72, 9>(static_cast<double>(i));

  // permute<0, 2, 1> -> swap last two dims
  auto p3 = permute<0, 2, 1>(t3);
  (void)p3;

  // t3(0, 1, 0) = 2. p3(0, 0, 1) should be 2.
  assert(p3(0, 0, 1).to_double() == 2.0);

  std::cout << "permute test passed" << std::endl;
  return 0;
}
