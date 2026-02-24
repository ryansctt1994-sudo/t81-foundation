#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include "t81/types/T81Float.hpp"

using namespace t81;
using F = T81Float<27, 9>;

void check(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << "\n";
    std::exit(1);
  }
}

void test_addition() {
  std::cout << "Testing Addition...\n";
  F one = F::from_double(1.0);
  F two = F::from_double(2.0);
  F three = one + two;
  check(std::abs(three.to_double() - 3.0) < 1e-9, "1.0 + 2.0 == 3.0");

  F zero = F::zero();
  check((one + zero).to_double() == 1.0, "1.0 + 0.0 == 1.0");
  check((zero + one).to_double() == 1.0, "0.0 + 1.0 == 1.0");

  F inf = F::inf();
  check((inf + one).is_inf() && !(inf + one).is_negative(), "Inf + 1.0 is Inf");
  check((one + inf).is_inf() && !(one + inf).is_negative(), "1.0 + Inf is Inf");
  check((inf + inf).is_inf() && !(inf + inf).is_negative(), "Inf + Inf is Inf");

  F neg_inf = -inf;
  check((neg_inf + one).is_inf() && (neg_inf + one).is_negative(), "-Inf + 1.0 is -Inf");
  check((one + neg_inf).is_inf() && (one + neg_inf).is_negative(), "1.0 + -Inf is -Inf");
  check((neg_inf + neg_inf).is_inf() && (neg_inf + neg_inf).is_negative(), "-Inf + -Inf is -Inf");

  check((inf + neg_inf).is_nae(), "Inf + -Inf is NaE");
  check((neg_inf + inf).is_nae(), "-Inf + Inf is NaE");

  F nae = F::nae();
  check((nae + one).is_nae(), "NaE + 1.0 is NaE");
  check((one + nae).is_nae(), "1.0 + NaE is NaE");
  check((nae + inf).is_nae(), "NaE + Inf is NaE");
}

void test_subtraction() {
  std::cout << "Testing Subtraction...\n";
  F one = F::from_double(1.0);
  F two = F::from_double(2.0);
  F neg_one = one - two;
  check(std::abs(neg_one.to_double() - -1.0) < 1e-9, "1.0 - 2.0 == -1.0");

  F zero = F::zero();
  check((one - zero).to_double() == 1.0, "1.0 - 0.0 == 1.0");
  check((zero - one).to_double() == -1.0, "0.0 - 1.0 == -1.0");

  F inf = F::inf();
  check((inf - one).is_inf() && !(inf - one).is_negative(), "Inf - 1.0 is Inf");
  check((one - inf).is_inf() && (one - inf).is_negative(), "1.0 - Inf is -Inf");

  F neg_inf = -inf;
  check((inf - neg_inf).is_inf() && !(inf - neg_inf).is_negative(), "Inf - (-Inf) is Inf");
  check((neg_inf - inf).is_inf() && (neg_inf - inf).is_negative(), "-Inf - Inf is -Inf");

  check((inf - inf).is_nae(), "Inf - Inf is NaE");

  F nae = F::nae();
  check((nae - one).is_nae(), "NaE - 1.0 is NaE");
  check((one - nae).is_nae(), "1.0 - NaE is NaE");
}

void test_multiplication() {
  std::cout << "Testing Multiplication...\n";
  F two = F::from_double(2.0);
  F three = F::from_double(3.0);
  F six = two * three;
  check(std::abs(six.to_double() - 6.0) < 1e-9, "2.0 * 3.0 == 6.0");

  F zero = F::zero();
  check((two * zero).is_zero(), "2.0 * 0.0 is 0.0");
  check((zero * two).is_zero(), "0.0 * 2.0 is 0.0");

  F inf = F::inf();
  check((inf * two).is_inf() && !(inf * two).is_negative(), "Inf * 2.0 is Inf");
  check((two * inf).is_inf() && !(two * inf).is_negative(), "2.0 * Inf is Inf");
  check((inf * inf).is_inf() && !(inf * inf).is_negative(), "Inf * Inf is Inf");

  F neg_inf = -inf;
  check((inf * neg_inf).is_inf() && (inf * neg_inf).is_negative(), "Inf * -Inf is -Inf");
  check((neg_inf * neg_inf).is_inf() && !(neg_inf * neg_inf).is_negative(), "-Inf * -Inf is Inf");

  // Inf * 0 is NaE
  check((inf * zero).is_nae(), "Inf * 0.0 is NaE");
  check((zero * inf).is_nae(), "0.0 * Inf is NaE");

  F nae = F::nae();
  check((nae * two).is_nae(), "NaE * 2.0 is NaE");
  check((two * nae).is_nae(), "2.0 * NaE is NaE");
}

void test_division() {
  std::cout << "Testing Division...\n";
  F six = F::from_double(6.0);
  F two = F::from_double(2.0);
  F three = six / two;
  check(std::abs(three.to_double() - 3.0) < 1e-9, "6.0 / 2.0 == 3.0");

  F zero = F::zero();
  // 0 / 2 -> 0
  check((zero / two).is_zero(), "0.0 / 2.0 is 0.0");

  F inf = F::inf();
  // Finite / 0 -> Inf
  check((two / zero).is_inf() && !(two / zero).is_negative(), "2.0 / 0.0 is Inf");
  F neg_two = -two;
  check((neg_two / zero).is_inf() && (neg_two / zero).is_negative(), "-2.0 / 0.0 is -Inf");

  // Inf / 0 -> NaE (implementation specific)
  check((inf / zero).is_nae(), "Inf / 0.0 is NaE");

  // 0 / 0 -> NaE
  check((zero / zero).is_nae(), "0.0 / 0.0 is NaE");

  // Finite / Inf -> 0
  check((two / inf).is_zero(), "2.0 / Inf is 0.0");
  check((neg_two / inf).is_zero(), "-2.0 / Inf is 0.0");

  // Inf / Inf -> NaE
  check((inf / inf).is_nae(), "Inf / Inf is NaE");

  F nae = F::nae();
  check((nae / two).is_nae(), "NaE / 2.0 is NaE");
  check((two / nae).is_nae(), "2.0 / NaE is NaE");
}

void test_fma() {
  std::cout << "Testing FMA...\n";
  F two = F::from_double(2.0);
  F three = F::from_double(3.0);
  F four = F::from_double(4.0);

  // 2 * 3 + 4 = 10
  F res = fma(two, three, four);
  check(std::abs(res.to_double() - 10.0) < 1e-9, "2.0 * 3.0 + 4.0 == 10.0");

  // (Inf * 2) + 4 -> Inf
  F inf = F::inf();
  res = fma(inf, two, four);
  check(res.is_inf(), "Inf * 2.0 + 4.0 is Inf");

  // (Inf * 0) + 4 -> NaE + 4 -> NaE
  F zero = F::zero();
  res = fma(inf, zero, four);
  check(res.is_nae(), "Inf * 0.0 + 4.0 is NaE");
}

int main() {
  test_addition();
  test_subtraction();
  test_multiplication();
  test_division();
  test_fma();
  std::cout << "All T81Float arithmetic tests PASSED!\n";
  return 0;
}
