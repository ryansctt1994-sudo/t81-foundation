#include <cstdint>
#include <iostream>
#include <random>

#include "t81/bigint.hpp"
#include "t81/bigint/divmod.hpp"

int main() {
  using t81::T81BigInt;
  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "bigint_properties_test failure: " << msg << "\n";
      return false;
    }
    return true;
  };

  // Fixed seed for deterministic replay.
  std::mt19937_64 rng(0x815A5A5AULL);

  // Phase 1: Small range (fast, many iterations)
  std::uniform_int_distribution<std::int64_t> dist_small(-1'000'000, 1'000'000);

  auto run_test_loop = [&](int iterations, std::uniform_int_distribution<std::int64_t>& dist, const char* phase_name) {
    std::cout << "Running " << phase_name << "...\n";
    for (int i = 0; i < iterations; ++i) {
      const std::int64_t ai = dist(rng);
      const std::int64_t bi = dist(rng);

      const T81BigInt a = T81BigInt::from_i64(ai);
      const T81BigInt b = T81BigInt::from_i64(bi);

      // Add/sub inverse law: (a + b) - b == a
      if (!expect(T81BigInt::sub(T81BigInt::add(a, b), b) == a, "add/sub inverse law failed")) return false;

      // Negation involution: -(-a) == a
      if (!expect(T81BigInt::neg(T81BigInt::neg(a)) == a, "double negation failed")) return false;

      // Multiplicative identity.
      if (!expect(T81BigInt::mul(a, T81BigInt::one()) == a, "multiplicative identity failed")) return false;

      if (!T81BigInt::is_zero(b)) {
        // Division identity via divmod: a = q*b + r.
        const auto dm = t81::divmod(a, b);
        const T81BigInt recomposed =
            T81BigInt::add(T81BigInt::mul(dm.q, b), dm.r);
        if (!expect(recomposed == a, "divmod recomposition failed")) return false;

        // GCD divisibility invariants.
        const T81BigInt g = T81BigInt::gcd(a, b);
        if (!T81BigInt::is_zero(g)) {
          if (!expect(T81BigInt::is_zero(T81BigInt::mod(a, g)), "gcd divisibility failed for lhs")) return false;
          if (!expect(T81BigInt::is_zero(T81BigInt::mod(b, g)), "gcd divisibility failed for rhs")) return false;
        }
      }
    }
    return true;
  };

  if (!run_test_loop(2500, dist_small, "small range")) return 1;

  // Phase 2: Large range (full int64 range, fewer iterations to avoid timeout if slow)
  // This ensures we test deep multi-limb logic and slow-path division.
  std::uniform_int_distribution<std::int64_t> dist_large(std::numeric_limits<std::int64_t>::min(), std::numeric_limits<std::int64_t>::max());
  if (!run_test_loop(100, dist_large, "large range")) return 1;

  std::cout << "bigint_properties_test ok\n";
  return 0;
}
