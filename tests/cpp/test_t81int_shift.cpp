#include <cassert>
#include <iostream>
#include <string>
#include "t81/core/T81Int.hpp"

using namespace t81;

// Basic helper to check if a value matches expected int64
template <std::size_t N>
void check_eq(const T81Int<N>& val, std::int64_t expected, const std::string& msg) {
  if (val.to_int64() != expected) {
    std::cerr << "FAIL: " << msg << " | Expected " << expected << ", got " << val.to_int64()
              << "\n";
    assert(false);
  }
}

void test_left_shift() {
  std::cout << "Testing Left Shift..." << std::endl;

  // T81Int<5> has 5 trits. Max value roughly 3^5 / 2 = 243 / 2 = 121.
  // 1 represented as ...001 (all Z except last P)
  T81Int<5> one(1);

  // 1 << 0 == 1
  check_eq(one << 0, 1, "1 << 0");

  // 1 << 1 == 3
  check_eq(one << 1, 3, "1 << 1");

  // 1 << 2 == 9
  check_eq(one << 2, 9, "1 << 2");

  // 1 << 4 == 3^4 = 81
  check_eq(one << 4, 81, "1 << 4");

  // 1 << 5 (out of bounds for N=5) -> should be 0 because all trits shifted out
  check_eq(one << 5, 0, "1 << 5");

  // 1 << 6 -> 0
  check_eq(one << 6, 0, "1 << 6");

  // Negative number
  T81Int<5> neg_one(-1);
  // -1 << 1 == -3
  check_eq(neg_one << 1, -3, "-1 << 1");

  // Zero
  T81Int<5> zero(0);
  check_eq(zero << 1, 0, "0 << 1");

  // Pattern shift
  // 5 = 1*3^1 + 2*3^0 = 1*3 + (-1) = 2 (No, 5 = 9 - 3 - 1)
  // 5 = 1*9 + (-1)*3 + (-1)*1 = 9-3-1 = 5. Trits: P N N. (indices 2, 1, 0)
  T81Int<10> five(5);
  // 5 << 1 = 15
  // P N N (5) -> P N N 0 (15) = 27 - 9 - 3 = 15. Correct.
  check_eq(five << 1, 15, "5 << 1");

  // 5 << 2 = 45
  // P N N (5) -> P N N 0 0 (45) = 81 - 27 - 9 = 45. Correct.
  check_eq(five << 2, 45, "5 << 2");
}

void test_right_shift() {
  std::cout << "Testing Right Shift..." << std::endl;

  T81Int<5> val(27);  // 3^3 = 27 (1 0 0 0)

  // 27 >> 0 = 27
  check_eq(val >> 0, 27, "27 >> 0");

  // 27 >> 1 = 9
  check_eq(val >> 1, 9, "27 >> 1");

  // 27 >> 2 = 3
  check_eq(val >> 2, 3, "27 >> 2");

  // 27 >> 3 = 1
  check_eq(val >> 3, 1, "27 >> 3");

  // 27 >> 4 = 0
  check_eq(val >> 4, 0, "27 >> 4");

  // Negative
  T81Int<5> neg_val(-27);
  check_eq(neg_val >> 1, -9, "-27 >> 1");

  // Rounding behavior
  // 5 = P N N.
  // >> 1 => P N (2).
  T81Int<10> five(5);
  check_eq(five >> 1, 2, "5 >> 1");

  // 2 = P N.
  // >> 1 => P (1).
  T81Int<5> two(2);
  check_eq(two >> 1, 1, "2 >> 1");

  // -2 = N P.
  // >> 1 => N (-1).
  T81Int<5> neg_two(-2);
  check_eq(neg_two >> 1, -1, "-2 >> 1");

  // 4 = P P.
  // >> 1 => P (1).
  T81Int<5> four(4);
  check_eq(four >> 1, 1, "4 >> 1");

  // Large shift -> 0
  check_eq(five >> 10, 0, "5 >> 10");
}

void test_assignment_operators() {
  std::cout << "Testing Assignment Operators..." << std::endl;

  T81Int<10> a(1);
  a <<= 2;
  check_eq(a, 9, "a <<= 2");

  a >>= 1;
  check_eq(a, 3, "a >>= 1");

  T81Int<10> b(5);
  b >>= 1;
  check_eq(b, 2, "b >>= 1");
}

void test_large_shift_trits() {
  std::cout << "Testing Large Shift (Trits)..." << std::endl;

  // Check that bits move correctly across large range
  T81Int<100> large(1);
  large <<= 50;
  // 1 at pos 0 moved to pos 50.
  if (large[50] != Trit::P) {
    std::cerr << "FAIL: large <<= 50, trit 50 not P\n";
    assert(false);
  }
  if (large[0] != Trit::Z) {
    std::cerr << "FAIL: large <<= 50, trit 0 not Z\n";
    assert(false);
  }

  large >>= 50;
  if (large[0] != Trit::P) {
    std::cerr << "FAIL: large >>= 50, trit 0 not P\n";
    assert(false);
  }
  if (large[50] != Trit::Z) {
    std::cerr << "FAIL: large >>= 50, trit 50 not Z\n";
    assert(false);
  }
  check_eq(large, 1, "large back to 1");
}

int main() {
  test_left_shift();
  test_right_shift();
  test_assignment_operators();
  test_large_shift_trits();
  std::cout << "All T81Int shift tests passed!" << std::endl;
  return 0;
}
