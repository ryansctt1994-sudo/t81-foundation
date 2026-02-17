#include <cassert>
#include <iostream>
#include "t81/core/T81Int.hpp"

using namespace t81;

int main() {
  using I = T81Int<18>;

  assert(gcd(I(12), I(18)).to_int64() == 6);
  assert(gcd(I(-12), I(18)).to_int64() == 6);
  assert(gcd(I(12), I(-18)).to_int64() == 6);
  assert(gcd(I(5), I(0)).to_int64() == 5);

  assert(lcm(I(12), I(18)).to_int64() == 36);
  assert(lcm(I(-12), I(18)).to_int64() == 36);
  assert(lcm(I(5), I(0)).to_int64() == 0);

  std::cout << "gcd lcm test passed" << std::endl;
  return 0;
}
