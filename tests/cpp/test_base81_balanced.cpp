#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "t81/codec/base81_balanced.hpp"
#include "t81/types/T81Int.hpp"

using namespace t81;
using namespace t81::codec::base81_balanced;

#define EXPECT(cond)                                                                    \
  if (!(cond)) {                                                                        \
    std::cerr << "Assertion failed: " << #cond << " at " << __FILE__ << ":" << __LINE__ \
              << std::endl;                                                             \
    std::exit(1);                                                                       \
  }

void test_single_digit() {
  std::cout << "Testing single digit pack/unpack..." << std::endl;
  for (int i = -40; i <= 40; ++i) {
    Trit trits[4];
    unpack_digit(static_cast<int8_t>(i), trits);
    int8_t repacked = pack_digit(trits);
    if (i != repacked) {
      std::cerr << "Fail: " << i << " -> trits -> " << (int)repacked << std::endl;
      std::exit(1);
    }
  }
  std::cout << "OK" << std::endl;
}

void test_carry_behavior() {
  std::cout << "Testing carry behavior..." << std::endl;
  // Example: 40 + 1 = 41. In Base-81 balanced, this should be (1, -40)
  // because 1*81 - 40 = 41.
  int8_t d1 = 40;
  int8_t d2 = 1;
  int sum = (int)d1 + (int)d2;
  int carry = 0;
  int digit = sum;
  if (digit > 40) {
    digit -= 81;
    carry = 1;
  } else if (digit < -40) {
    digit += 81;
    carry = -1;
  }
  EXPECT(carry == 1);
  EXPECT(digit == -40);

  // Verify it matches trits
  Trit t1[4], t2[4];
  unpack_digit(d1, t1);  // should be (+, +, +, +) -> 1+3+9+27=40
  unpack_digit(d2, t2);  // should be (+, 0, 0, 0) -> 1

  Trit sum_trits[4];
  int c = 0;
  for (int i = 0; i < 4; ++i) {
    int s = (int)t1[i] + (int)t2[i] + c;
    if (s > 1) {
      s -= 3;
      c = 1;
    } else if (s < -1) {
      s += 3;
      c = -1;
    } else {
      c = 0;
    }
    sum_trits[i] = (Trit)s;
  }
  EXPECT(c == carry);
  EXPECT(pack_digit(sum_trits) == (int8_t)digit);

  std::cout << "OK" << std::endl;
}

void test_vector_roundtrip() {
  std::cout << "Testing vector roundtrip..." << std::endl;
  std::vector<Trit> trits = {Trit::P, Trit::N, Trit::Z, Trit::P, Trit::P,
                             Trit::P, Trit::N, Trit::N, Trit::Z};
  auto digits_res = pack_vector(trits);
  EXPECT(digits_res.is_ok());
  auto digits = digits_res.value();

  EXPECT(digits.size() == 3);  // (9 + 3) / 4 = 3

  auto unpacked_res = unpack_vector(digits, trits.size());
  EXPECT(unpacked_res.is_ok());
  auto unpacked = unpacked_res.value();

  EXPECT(unpacked.size() == trits.size());
  for (size_t i = 0; i < trits.size(); ++i) {
    EXPECT(unpacked[i] == trits[i]);
  }
  std::cout << "OK" << std::endl;
}

void test_conversion() {
  std::cout << "Testing unbalanced <-> balanced conversion..." << std::endl;
  std::vector<uint8_t> unbalanced = {0, 40, 80};
  std::vector<int8_t> balanced(3);
  to_balanced(unbalanced, balanced);
  EXPECT(balanced[0] == -40);
  EXPECT(balanced[1] == 0);
  EXPECT(balanced[2] == 40);

  std::vector<uint8_t> back(3);
  to_unbalanced(balanced, back);
  EXPECT(back[0] == 0);
  EXPECT(back[1] == 40);
  EXPECT(back[2] == 80);
  std::cout << "OK" << std::endl;
}

int main() {
  try {
    test_single_digit();
    test_vector_roundtrip();
    test_conversion();
    test_carry_behavior();
    std::cout << "All Balanced Base-81 tests passed!" << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
