#include <cstdlib>
#include <iostream>
#include <tuple>
#include "t81/types/T81BigInt.hpp"

using namespace t81::v1;

void check(bool cond, const char* msg) {
  if (!cond) {
    std::cerr << "Check failed: " << msg << "\n";
    std::exit(1);
  }
}

void test_extended_gcd() {
  std::cout << "Testing extended_gcd...\n";
  // gcd(10, 6) = 2. 10*(-1) + 6*2 = -10 + 12 = 2.
  auto [g, x, y] = T81BigInt::extended_gcd(T81BigInt(10), T81BigInt(6));

  check(g == T81BigInt(2), "gcd(10, 6) == 2");
  check(T81BigInt(10) * x + T81BigInt(6) * y == g, "10x + 6y = g");

  // gcd(240, 46) = 2. 240*(-9) + 46*47 = -2160 + 2162 = 2.
  auto [g2, x2, y2] = T81BigInt::extended_gcd(T81BigInt(240), T81BigInt(46));
  check(g2 == T81BigInt(2), "gcd(240, 46) == 2");
  check(T81BigInt(240) * x2 + T81BigInt(46) * y2 == g2, "240x + 46y = g");

  // gcd(17, 13) = 1.
  auto [g3, x3, y3] = T81BigInt::extended_gcd(T81BigInt(17), T81BigInt(13));
  check(g3 == T81BigInt(1), "gcd(17, 13) == 1");
  check(T81BigInt(17) * x3 + T81BigInt(13) * y3 == g3, "17x + 13y = g");
}

void test_modular_inverse() {
  std::cout << "Testing modular_inverse...\n";

  // 3 * x = 1 (mod 11). x = 4. 3*4 = 12 = 1 (mod 11).
  T81BigInt inv = T81BigInt::modular_inverse(T81BigInt(3), T81BigInt(11));
  check(inv == T81BigInt(4), "inv(3, 11) == 4");

  // 10 * x = 1 (mod 17). x = 12. 10*12 = 120 = 17*7 + 1.
  inv = T81BigInt::modular_inverse(T81BigInt(10), T81BigInt(17));
  check(inv == T81BigInt(12), "inv(10, 17) == 12");

  // Failure case: gcd(6, 9) = 3 != 1.
  try {
    T81BigInt::modular_inverse(T81BigInt(6), T81BigInt(9));
    check(false, "Should have thrown for non-coprime inputs");
  } catch (const std::domain_error&) {
    // Expected
  }

  // Failure case: mod <= 1
  try {
    T81BigInt::modular_inverse(T81BigInt(5), T81BigInt(1));
    check(false, "Should have thrown for mod <= 1");
  } catch (const std::domain_error&) {
    // Expected
  }
}

int main() {
  test_extended_gcd();
  test_modular_inverse();
  std::cout << "All tests passed.\n";
  return 0;
}
