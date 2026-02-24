#include <cassert>
#include <iostream>
#include "t81/types/T81Polynomial.hpp"

using namespace t81;

void test_construction() {
  FloatPoly p{T81Float72_9(1.0), T81Float72_9(2.0), T81Float72_9(3.0)};
  assert(p.degree() == 2);
  assert(std::abs(p[0].to_double() - 1.0) < 1e-6);
  std::cout << "test_construction PASSED\n";
}

int main() {
  test_construction();
  return 0;
}
