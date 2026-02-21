#include <cassert>
#include <iostream>
#include <random>
#include <vector>

#include "t81/bigint.hpp"
#include "t81/core/T81Int.hpp"

using namespace t81::v1;

// Helper to generate random BigInt
T81BigInt random_bigint(std::mt19937_64& rng, int max_limbs) {
  std::uniform_int_distribution<int> dist_len(1, max_limbs);
  int len = dist_len(rng);

  T81BigInt res(0);
  std::uniform_int_distribution<int64_t> dist_val;

  // Base 2^60
  T81BigInt shift(1LL << 60);

  for (int i = 0; i < len; ++i) {
    res = res * shift + T81BigInt(dist_val(rng) & 0x0FFFFFFFFFFFFFFF);
  }

  // Random sign
  if (dist_val(rng) % 2) res = -res;
  return res;
}

// Helper for T81Int
template <std::size_t N>
t81::T81Int<N> random_t81int(std::mt19937_64& rng) {
  // Safe range for T81Int<40> (approx +/- 6e18)
  // We use +/- 4e18 to be safe and cover a good range
  std::uniform_int_distribution<int64_t> dist(-4000000000000000000LL,
                                              4000000000000000000LL);
  t81::T81Int<N> val(dist(rng));
  return val;
}

int main() {
  std::mt19937_64 rng(54321);  // Different seed from fuzz_bigint_div
  int iterations = 10000;

  std::cout << "Running property-based tests for " << iterations
            << " iterations...\n";

  for (int i = 0; i < iterations; ++i) {
    T81BigInt a = random_bigint(rng, 4);  // Up to ~240 bits
    T81BigInt b = random_bigint(rng, 4);
    T81BigInt c = random_bigint(rng, 4);

    // 1. Commutativity of Addition
    if (a + b != b + a) {
      std::cerr << "FAILED: Commutativity of Addition\n"
                << "a=" << a.to_string() << "\n"
                << "b=" << b.to_string() << "\n";
      return 1;
    }

    // 2. Associativity of Addition
    if ((a + b) + c != a + (b + c)) {
      std::cerr << "FAILED: Associativity of Addition\n"
                << "a=" << a.to_string() << "\n"
                << "b=" << b.to_string() << "\n"
                << "c=" << c.to_string() << "\n";
      return 1;
    }

    // 3. Commutativity of Multiplication
    if (a * b != b * a) {
      std::cerr << "FAILED: Commutativity of Multiplication\n"
                << "a=" << a.to_string() << "\n"
                << "b=" << b.to_string() << "\n";
      return 1;
    }

    // 4. Associativity of Multiplication
    if ((a * b) * c != a * (b * c)) {
      std::cerr << "FAILED: Associativity of Multiplication\n"
                << "a=" << a.to_string() << "\n"
                << "b=" << b.to_string() << "\n"
                << "c=" << c.to_string() << "\n";
      return 1;
    }

    // 5. Distributivity of Multiplication over Addition
    if (a * (b + c) != (a * b) + (a * c)) {
      std::cerr << "FAILED: Distributivity\n"
                << "a=" << a.to_string() << "\n"
                << "b=" << b.to_string() << "\n"
                << "c=" << c.to_string() << "\n";
      return 1;
    }

    // 6. Identity Elements
    if (a + T81BigInt(0) != a) {
      std::cerr << "FAILED: Additive Identity\n"
                << "a=" << a.to_string() << "\n";
      return 1;
    }
    if (a * T81BigInt(1) != a) {
      std::cerr << "FAILED: Multiplicative Identity\n"
                << "a=" << a.to_string() << "\n";
      return 1;
    }

    // 7. Additive Inverse
    if (a + (-a) != T81BigInt(0)) {
      std::cerr << "FAILED: Additive Inverse\n"
                << "a=" << a.to_string() << "\n";
      return 1;
    }

    // 8. Euclidean Division Properties
    if (!b.is_zero()) {
      auto [q, r] = T81BigInt::div_mod(a, b);

      // a = q * b + r
      if (a != q * b + r) {
        std::cerr << "FAILED: Euclidean Division Equation a = q*b + r\n"
                  << "a=" << a.to_string() << "\n"
                  << "b=" << b.to_string() << "\n"
                  << "q=" << q.to_string() << "\n"
                  << "r=" << r.to_string() << "\n";
        return 1;
      }

      // 0 <= r < |b|
      if (r.is_negative()) {
        std::cerr << "FAILED: Euclidean Division Remainder Negative\n"
                  << "r=" << r.to_string() << "\n";
        return 1;
      }
      if (r >= b.abs()) {
        std::cerr << "FAILED: Euclidean Division Remainder Too Large\n"
                  << "r=" << r.to_string() << "\n"
                  << "|b|=" << b.abs().to_string() << "\n";
        return 1;
      }
    }

    // 9. T81Int <-> T81BigInt Roundtrip
    {
      auto t81val = random_t81int<40>(rng);  // 40 trits fits in int64
      T81BigInt bigval(t81val);
      auto back = bigval.try_to_int<40>();
      if (!back || *back != t81val) {
        std::cerr << "FAILED: T81Int <-> T81BigInt Roundtrip\n";
        return 1;
      }
    }
  }

  std::cout << "All property tests passed!\n";
  return 0;
}
// test
