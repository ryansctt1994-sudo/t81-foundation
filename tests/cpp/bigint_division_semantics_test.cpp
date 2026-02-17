#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "t81/bigint.hpp"

int main() {
  using t81::T81BigInt;

  const auto require = [](bool cond, const char* msg) {
    if (!cond) {
      std::cerr << "bigint_division_semantics_test failed: " << msg << "\n";
      std::abort();
    }
  };

  // Legacy fast-path: |a| < |b| returns 0.
  require(T81BigInt::div(T81BigInt::from_i64(3), T81BigInt::from_i64(10)) == T81BigInt::zero(),
          "|a|<|b| positive");
  require(
      T81BigInt::div(T81BigInt::from_i64(-3), T81BigInt::from_i64(10)) == T81BigInt::from_i64(-1),
      "|a|<|b| negative dividend");
  require(T81BigInt::div(T81BigInt::from_i64(3), T81BigInt::from_i64(-10)) == T81BigInt::zero(),
          "|a|<|b| negative divisor");

  // Exact division succeeds with expected sign.
  require(T81BigInt::div(T81BigInt::from_i64(81), T81BigInt::from_i64(9)) == T81BigInt::from_i64(9),
          "exact positive");
  require(
      T81BigInt::div(T81BigInt::from_i64(-81), T81BigInt::from_i64(9)) == T81BigInt::from_i64(-9),
      "exact negative dividend");
  require(
      T81BigInt::div(T81BigInt::from_i64(-81), T81BigInt::from_i64(-9)) == T81BigInt::from_i64(9),
      "exact both negative");

  // Non-exact division with |a| >= |b| returns quotient.
  require(T81BigInt::div(T81BigInt::from_i64(10), T81BigInt::from_i64(3)) == T81BigInt::from_i64(3),
          "non-exact division");

  // Division by zero throws.
  bool threw = false;
  try {
    (void)T81BigInt::div(T81BigInt::from_i64(10), T81BigInt::zero());
  } catch (const std::domain_error&) {
    threw = true;
  }
  require(threw, "div by zero must throw");

  std::cout << "bigint_division_semantics_test ok\n";
  return 0;
}
