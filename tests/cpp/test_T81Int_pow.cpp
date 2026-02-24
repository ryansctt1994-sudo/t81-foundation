#include <cassert>
#include <iostream>
#include "t81/types/T81Int.hpp"

using namespace t81;

int main() {
  using I = T81Int<18>;

  I base(3);
  I exp(4);

  // 3^4 = 81
  assert(pow(base, exp).to_int64() == 81);

  // 3^0 = 1
  assert(pow(base, I(0)).to_int64() == 1);

  // 0^5 = 0
  assert(pow(I(0), I(5)).to_int64() == 0);

  // -2^3 = -8
  assert(pow(I(-2), I(3)).to_int64() == -8);

  // -2^2 = 4
  assert(pow(I(-2), I(2)).to_int64() == 4);

  std::cout << "int pow test passed" << std::endl;
  return 0;
}
