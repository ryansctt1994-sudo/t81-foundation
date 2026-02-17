#include <cassert>
#include <iostream>
#include "t81/core/T81Tensor.hpp"

using namespace t81;

int main() {
  using Tensor2x3 = T81Tensor<T81Float<72, 9>, 2, 2, 3>;
  Tensor2x3 t;
  // Fill
  for (size_t i = 0; i < 6; ++i) t.data[i] = T81Float<72, 9>(static_cast<double>(i));

  // Slice first row
  auto s1 = slice<0, 1>(t);
  // Should be 1x3
  static_assert(decltype(s1)::rank() == 2);
  static_assert(decltype(s1)::shape()[0] == 1);
  static_assert(decltype(s1)::shape()[1] == 3);

  assert(s1(0, 0).to_double() == 0.0);
  assert(s1(0, 1).to_double() == 1.0);
  assert(s1(0, 2).to_double() == 2.0);

  // Slice second row
  auto s2 = slice<1, 2>(t);
  assert(s2(0, 0).to_double() == 3.0);

  std::cout << "slice test passed" << std::endl;
  return 0;
}
