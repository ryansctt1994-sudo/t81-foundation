#include <iostream>
#include <limits>
#include <stdexcept>
#include "t81/core/T81BigInt.hpp"
#include "t81/core/T81Int.hpp"

// Simple assertion helper
#define ASSERT_THROWS(expr, ExType)                                         \
  try {                                                                     \
    expr;                                                                   \
    std::cerr << "FAIL: " << #expr << " did not throw " << #ExType << "\n"; \
    return 1;                                                               \
  } catch (const ExType&) {                                                 \
    /* Expected */                                                          \
  } catch (...) {                                                           \
    std::cerr << "FAIL: " << #expr << " threw wrong exception\n";           \
    return 1;                                                               \
  }

#define ASSERT_EQ(a, b)                                  \
  if (!((a) == (b))) {                                   \
    std::cerr << "FAIL: " << #a << " != " << #b << "\n"; \
    return 1;                                            \
  }

int main() {
  using t81::T81Int;
  using t81::v1::T81BigInt;

  // Test 1: T81BigInt::to_int<N> overflow
  {
    std::cout << "Test 1: T81BigInt::to_int<10> overflow check...\n";
    // Construct a BigInt larger than 10 trits.
    // 10 trits max value is (3^10 - 1) / 2 = 29524.
    // 11 trits max value is (3^11 - 1) / 2 = 88573.
    T81BigInt big = T81BigInt(88573);
    // This fits in 11 trits but not 10?
    // Wait, T81BigInt::to_int<N> checks significant trits.
    // 88573 in balanced ternary: 11111111111 (11 ones) -> 88573?
    // sum(3^i) for i=0..10 = (3^11 - 1)/2. Yes.
    // So it has 11 significant trits.

    ASSERT_THROWS(big.to_int<10>(), std::overflow_error);
  }

  // Test 2: T81BigInt::to_int64 overflow
  {
    std::cout << "Test 2: T81BigInt::to_int64 overflow check...\n";
    // Max int64 is approx 9e18.
    // Let's make a very large BigInt.
    // 3^40 > 2^63.
    // T81BigInt uses limbs of 81 trits. One limb with high trits set should overflow int64.
    // We can construct it via multiplication.
    T81BigInt base(3);
    T81BigInt exp(100);
    T81BigInt huge = T81BigInt::pow(base, exp);

    ASSERT_THROWS(huge.to_int64(), std::overflow_error);
  }

  // Test 3: T81BigInt::try_to_int<N>
  {
    std::cout << "Test 3: T81BigInt::try_to_int<10>...\n";
    T81BigInt big(88573);  // 11 trits
    auto res = big.try_to_int<10>();
    if (res.has_value()) {
      std::cerr << "FAIL: try_to_int<10> should return nullopt for 11-trit value\n";
      return 1;
    }

    T81BigInt small(100);  // Fits in 10 trits
    auto res2 = small.try_to_int<10>();
    if (!res2.has_value()) {
      std::cerr << "FAIL: try_to_int<10> should return value for 100\n";
      return 1;
    }
    ASSERT_EQ(res2->to_int64(), 100);
  }

  // Test 4: T81Int::try_to_int<K> (demotion)
  {
    std::cout << "Test 4: T81Int::try_to_int<K>...\n";
    T81Int<10> val(100);  // 100 = 1*81 + 19 = 1*81 + 2*9 + 1 (10201 base 3 balanced? no)
    // 100 = 81 + 9 + 9 + 1 = 1*3^4 + 2*3^2 + 1.
    // Trits needed: 5. (3^4=81).

    // Demote to 5 trits: should succeed
    auto res = val.try_to_int<5>();
    if (!res.has_value()) {
      std::cerr << "FAIL: T81Int<10>(100).try_to_int<5> failed\n";
      return 1;
    }
    ASSERT_EQ(res->to_int64(), 100);

    // Demote to 4 trits (max 40): should fail (100 > 40)
    auto res2 = val.try_to_int<4>();
    if (res2.has_value()) {
      std::cerr << "FAIL: T81Int<10>(100).try_to_int<4> succeeded unexpectedly\n";
      return 1;
    }
  }

  // Test 5: T81Int::checked_to_int<K> overflow
  {
    std::cout << "Test 5: T81Int::checked_to_int<K> overflow...\n";
    T81Int<10> val(100);
    ASSERT_THROWS(val.checked_to_int<4>(), std::overflow_error);

    // Valid checked cast
    try {
      auto v = val.checked_to_int<5>();
      ASSERT_EQ(v.to_int64(), 100);
    } catch (...) {
      std::cerr << "FAIL: checked_to_int<5> threw unexpectedly\n";
      return 1;
    }
  }

  std::cout << "PASS\n";
  return 0;
}
