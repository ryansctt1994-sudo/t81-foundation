#include <cstdlib>
#include <iostream>
#include "t81/core/T81BigInt.hpp"

using namespace t81::v1;

void check(bool cond, const char* msg) {
  if (!cond) {
    std::cerr << "Check failed: " << msg << "\n";
    std::exit(1);
  }
}

void test_sqrt() {
  check(T81BigInt::sqrt(T81BigInt(0)) == T81BigInt(0), "sqrt(0)");
  check(T81BigInt::sqrt(T81BigInt(1)) == T81BigInt(1), "sqrt(1)");
  check(T81BigInt::sqrt(T81BigInt(4)) == T81BigInt(2), "sqrt(4)");
  check(T81BigInt::sqrt(T81BigInt(9)) == T81BigInt(3), "sqrt(9)");

  check(T81BigInt::sqrt(T81BigInt(2)) == T81BigInt(1), "sqrt(2)");
  check(T81BigInt::sqrt(T81BigInt(3)) == T81BigInt(1), "sqrt(3)");
  check(T81BigInt::sqrt(T81BigInt(8)) == T81BigInt(2), "sqrt(8)");

  // Large square
  // 12345^2 = 152399025
  T81BigInt sq(152399025);
  check(T81BigInt::sqrt(sq) == T81BigInt(12345), "sqrt(12345^2)");

  std::cout << "test_sqrt passed\n";
}

int main() {
  test_sqrt();
  return 0;
}
