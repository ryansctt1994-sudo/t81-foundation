#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include "t81/core/T81Int.hpp"

using namespace t81;

namespace t81 {
std::ostream& operator<<(std::ostream& os, Trit t) {
  switch (t) {
    case Trit::N:
      os << "N";
      break;
    case Trit::Z:
      os << "Z";
      break;
    case Trit::P:
      os << "P";
      break;
  }
  return os;
}
}  // namespace t81

// Helper macro for assertions
#define ASSERT_EQ(a, b)                                                                \
  do {                                                                                 \
    if ((a) != (b)) {                                                                  \
      std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ << ": " << #a \
                << " != " << #b << " (" << (a) << " != " << (b) << ")\n";              \
      std::exit(1);                                                                    \
    }                                                                                  \
  } while (0)

template <std::size_t N>
void test_sign_trit() {
  std::cout << "Testing sign_trit for N=" << N << "...\n";
  using IntType = T81Int<N>;

  // 1. Zero
  IntType zero(0);
  ASSERT_EQ(zero.sign_trit(), Trit::Z);
  ASSERT_EQ(zero.significant_trits(), 0);

  // 2. Small Positive
  IntType one(1);
  ASSERT_EQ(one.sign_trit(), Trit::P);
  ASSERT_EQ(one.significant_trits(), 1);

  if constexpr (N >= 2) {
    IntType two(2);
    // 2 = 1T = 3^1*1 + 3^0*(-1) = 3-1=2. Trits: T, 1. MSB is 1 (P).
    ASSERT_EQ(two.sign_trit(), Trit::P);
    ASSERT_EQ(two.significant_trits(), 2);

    IntType three(3);  // 10 = 3^1*1 + 3^0*0 = 3. Trits: 0, 1. MSB is 1 (P).
    ASSERT_EQ(three.sign_trit(), Trit::P);
    ASSERT_EQ(three.significant_trits(), 2);

    // 3. Small Negative
    IntType neg_two(-2);  // T1 = 3^1*(-1) + 3^0*1 = -3+1=-2. Trits: 1, T. MSB is T (N).
    ASSERT_EQ(neg_two.sign_trit(), Trit::N);
    ASSERT_EQ(neg_two.significant_trits(), 2);
  }

  // Small Negative -1 fits in 1 trit (T)
  IntType neg_one(-1);
  ASSERT_EQ(neg_one.sign_trit(), Trit::N);
  ASSERT_EQ(neg_one.significant_trits(), 1);

  // 4. Max Value
  IntType max_val = IntType::kMaxValue;
  ASSERT_EQ(max_val.sign_trit(), Trit::P);
  ASSERT_EQ(max_val.significant_trits(), N);

  // 5. Min Value
  IntType min_val = IntType::kMinValue;
  ASSERT_EQ(min_val.sign_trit(), Trit::N);
  ASSERT_EQ(min_val.significant_trits(), N);

  // 6. Manual Trit Manipulation
  IntType manual(0);
  // Set highest trit to P
  manual[N - 1] = Trit::P;
  ASSERT_EQ(manual.sign_trit(), Trit::P);
  ASSERT_EQ(manual.significant_trits(), N);

  // Set highest trit to N
  manual[N - 1] = Trit::N;
  ASSERT_EQ(manual.sign_trit(), Trit::N);
  ASSERT_EQ(manual.significant_trits(), N);

  // Set highest trit to Z, second highest to P
  if constexpr (N >= 2) {
    manual[N - 1] = Trit::Z;
    manual[N - 2] = Trit::P;
    ASSERT_EQ(manual.sign_trit(), Trit::P);
    ASSERT_EQ(manual.significant_trits(), N - 1);
  }
}

int main() {
  test_sign_trit<1>();
  test_sign_trit<2>();
  test_sign_trit<9>();
  test_sign_trit<18>();
  test_sign_trit<27>();
  test_sign_trit<40>();
  test_sign_trit<42>();  // Check larger than 64-bit int capability if applicable

  std::cout << "All sign_trit tests passed!\n";
  return 0;
}
