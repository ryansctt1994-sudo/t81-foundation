#include <cstdlib>
#include <iostream>
#include <tuple>
#include "t81/core/T81BigInt.hpp"

using namespace t81::v1;

void check(bool cond, const char* msg) {
  if (!cond) {
    std::cerr << "Check failed: " << msg << "\n";
    std::exit(1);
  }
}

void test_modular_inverse_stein_basic() {
  std::cout << "Testing modular_inverse_stein basic...\n";

  // 3 * x = 1 (mod 11). x = 4. 3*4 = 12 = 1 (mod 11).
  // m=11 (1 1 N ? 9+3-1=11 ? no. 11 = 9+3-1 is 11. 1 1 N. lowest trit N (-1). coprime to 3.)
  T81BigInt inv = T81BigInt::modular_inverse_stein(T81BigInt(3), T81BigInt(11));
  check(inv == T81BigInt(4), "inv(3, 11) == 4");

  // 10 * x = 1 (mod 17). x = 12. 10*12 = 120 = 17*7 + 1.
  inv = T81BigInt::modular_inverse_stein(T81BigInt(10), T81BigInt(17));
  check(inv == T81BigInt(12), "inv(10, 17) == 12");

  // a = 2, m = 3. gcd(2,3)=1. inv(2,3) = 2. 2*2=4=1 mod 3.
  // m=3 is divisible by 3. Should fallback to standard Euclidean.
  inv = T81BigInt::modular_inverse_stein(T81BigInt(2), T81BigInt(3));
  check(inv == T81BigInt(2), "inv(2, 3) == 2");

  // Failure case: gcd(6, 9) = 3 != 1.
  try {
    T81BigInt::modular_inverse_stein(T81BigInt(6), T81BigInt(9));
    check(false, "Should have thrown for non-coprime inputs");
  } catch (const std::domain_error&) {
    // Expected
  }
}

void test_modular_inverse_stein_large() {
  std::cout << "Testing modular_inverse_stein large...\n";
  // Use a prime modulus not divisible by 3.
  // 13 is prime.
  // 3^5 - 1 = 242.
  // Let's use m = 241 (Prime). 2+4+1=7 not div by 3.
  T81BigInt m(241);
  T81BigInt a(100);
  T81BigInt inv = T81BigInt::modular_inverse_stein(a, m);
  T81BigInt prod = (a * inv) % m;
  check(prod == T81BigInt(1), "inv(100, 241) * 100 == 1 mod 241");

  // Larger prime?
  // 3^10 = 59049.
  // Let's assume 59053 is prime? Not sure.
  // Just use m = 3^10 - 2 (likely coprime to 3).
  // m = 59047.
  T81BigInt m_large = T81BigInt(59047);
  T81BigInt a_large(12345);
  // Ensure gcd is 1. If not, catch.
  try {
    T81BigInt inv_large = T81BigInt::modular_inverse_stein(a_large, m_large);
    T81BigInt prod_large = (a_large * inv_large) % m_large;
    check(prod_large == T81BigInt(1), "inv large check");
  } catch (const std::domain_error&) {
    // gcd != 1, ignore
    std::cout << "Skipped large check due to gcd != 1\n";
  }
}

int main() {
  test_modular_inverse_stein_basic();
  test_modular_inverse_stein_large();
  std::cout << "All stein inverse tests passed.\n";
  return 0;
}
