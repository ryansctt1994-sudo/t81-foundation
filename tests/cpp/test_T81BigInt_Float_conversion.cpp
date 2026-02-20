#include <cassert>
#include <cmath>
#include <iostream>
#include "t81/core/T81BigInt.hpp"
#include "t81/core/T81Float.hpp"

using namespace t81;
using namespace t81::v1;

void check(bool condition, const char* msg) {
  if (!condition) {
    std::cerr << "FAIL: " << msg << std::endl;
    std::exit(1);
  }
}

void test_from_float_basic() {
  std::cout << "Testing FromFloat Basic...\n";
  // 1.0
  auto f1 = T81Float<18, 9>::from_double(1.0);
  auto b1 = T81BigInt::from_float(f1);
  check(b1 == T81BigInt(1), "1.0 should convert to 1");

  // 3.0
  auto f3 = T81Float<18, 9>::from_double(3.0);
  auto b3 = T81BigInt::from_float(f3);
  check(b3 == T81BigInt(3), "3.0 should convert to 3");

  // 0.0
  auto f0 = T81Float<18, 9>::zero();
  auto b0 = T81BigInt::from_float(f0);
  check(b0 == T81BigInt(0), "0.0 should convert to 0");

  // -1.0
  auto fn1 = T81Float<18, 9>::from_double(-1.0);
  auto bn1 = T81BigInt::from_float(fn1);
  check(bn1 == T81BigInt(-1), "-1.0 should convert to -1");
}

void test_from_float_rounding() {
  std::cout << "Testing FromFloat Rounding...\n";
  // 1.4 -> 1
  auto f1_4 = T81Float<18, 9>::from_double(1.4);
  auto b1_4 = T81BigInt::from_float(f1_4);
  check(b1_4 == T81BigInt(1), "1.4 should round to 1");

  // 1.6 -> 2
  auto f1_6 = T81Float<18, 9>::from_double(1.6);
  auto b1_6 = T81BigInt::from_float(f1_6);
  check(b1_6 == T81BigInt(2), "1.6 should round to 2");

  // 1.5 -> 2 (Round half up)
  auto f1_5 = T81Float<18, 9>::from_double(1.5);
  auto b1_5 = T81BigInt::from_float(f1_5);
  check(b1_5 == T81BigInt(2), "1.5 should round to 2");

  // -1.5 -> -2
  auto fn1_5 = T81Float<18, 9>::from_double(-1.5);
  auto bn1_5 = T81BigInt::from_float(fn1_5);
  check(bn1_5 == T81BigInt(-2), "-1.5 should round to -2");

  // -1.4 -> -1
  auto fn1_4 = T81Float<18, 9>::from_double(-1.4);
  auto bn1_4 = T81BigInt::from_float(fn1_4);
  check(bn1_4 == T81BigInt(-1), "-1.4 should round to -1");
}

void test_to_float_basic() {
  std::cout << "Testing ToFloat Basic...\n";
  T81BigInt b1(1);
  auto f1 = b1.to_float<18, 9>();
  check(std::abs(f1.to_double() - 1.0) < 1e-9, "1 should convert to 1.0");

  T81BigInt b3(3);
  auto f3 = b3.to_float<18, 9>();
  check(std::abs(f3.to_double() - 3.0) < 1e-9, "3 should convert to 3.0");

  T81BigInt b0(0);
  auto f0 = b0.to_float<18, 9>();
  check(f0.is_zero(), "0 should convert to 0.0");

  T81BigInt bn1(-1);
  auto fn1 = bn1.to_float<18, 9>();
  check(std::abs(fn1.to_double() - -1.0) < 1e-9, "-1 should convert to -1.0");
}

void test_to_float_large() {
  std::cout << "Testing ToFloat Large...\n";
  // 3^20
  T81BigInt b = T81BigInt::pow(T81BigInt(3), T81BigInt(20));
  auto f = b.to_float<18, 9>();
  check(std::abs(f.to_double() - std::pow(3.0, 20.0)) < 1e-3, "3^20 conversion failed");

  // 3^20 + 1
  T81BigInt b2 = b + T81BigInt(1);
  auto f2 = b2.to_float<18, 9>();
  check(std::abs(f2.to_double() - std::pow(3.0, 20.0)) < 1e-3, "3^20 + 1 conversion failed");
}

void test_large_int_constructor() {
  std::cout << "Testing Large Int Constructor...\n";
  // Construct BigInt from large T81Int
  constexpr size_t N = 100;
  T81Int<N> large_int;
  large_int[99] = Trit::P;  // 3^99

  T81BigInt b(large_int);
  T81BigInt expected = T81BigInt::pow(T81BigInt(3), T81BigInt(99));

  check(b == expected, "Large T81Int constructor failed");
}

void test_to_int() {
  std::cout << "Testing ToInt...\n";

  // Test small T81Int constructor
  T81Int<10> small_int(42);
  T81BigInt b_small(small_int);
  check(b_small == T81BigInt(42), "T81BigInt(T81Int<10>(42)) failed");

  T81BigInt b(10);
  T81Int<18> i18 = b.to_int<18>();
  check(i18.to_int64() == 10, "10 should convert to T81Int<18>(10)");

  T81BigInt bn10(-10);
  T81Int<18> in18 = bn10.to_int<18>();
  check(in18.to_int64() == -10, "-10 should convert to T81Int<18>(-10)");

  // Test large int conversion
  constexpr size_t N = 100;
  T81BigInt bl = T81BigInt::pow(T81BigInt(3), T81BigInt(99));
  T81Int<N> il = bl.to_int<N>();
  check(il[99] == Trit::P, "3^99 should have P at index 99");
  check(il[98] == Trit::Z, "3^99 should have Z at index 98");

  // Test overflow
  try {
    bl.to_int<99>();  // Should fit? No, 3^99 needs 100 trits (0..99). T81Int<99> has 0..98.
    check(false, "Should throw overflow error for 3^99 -> T81Int<99>");
  } catch (const std::overflow_error&) {
    // Expected
  }
}

int main() {
  test_from_float_basic();
  test_from_float_rounding();
  test_to_float_basic();
  test_to_float_large();
  test_large_int_constructor();
  test_to_int();

  std::cout << "All T81BigInt-Float conversion tests PASSED!\n";
  return 0;
}
