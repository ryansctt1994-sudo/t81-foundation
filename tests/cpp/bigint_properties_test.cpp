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
  std::uniform_int_distribution<std::int64_t> dist(-1'000'000, 1'000'000);

  for (int i = 0; i < 2500; ++i) {
    const std::int64_t ai = dist(rng);
    const std::int64_t bi = dist(rng);

    const T81BigInt a = T81BigInt::from_i64(ai);
    const T81BigInt b = T81BigInt::from_i64(bi);

    // Add/sub inverse law: (a + b) - b == a
    if (!expect(T81BigInt::sub(T81BigInt::add(a, b), b) == a, "add/sub inverse law failed")) return 1;

    // Negation involution: -(-a) == a
    if (!expect(T81BigInt::neg(T81BigInt::neg(a)) == a, "double negation failed")) return 1;

    // Multiplicative identity.
    if (!expect(T81BigInt::mul(a, T81BigInt::one()) == a, "multiplicative identity failed")) return 1;

    if (!T81BigInt::is_zero(b)) {
      // Division identity via divmod: a = q*b + r.
      const auto dm = t81::divmod(a, b);
      const T81BigInt recomposed =
          T81BigInt::add(T81BigInt::mul(dm.q, b), dm.r);
      if (!expect(recomposed == a, "divmod recomposition failed")) return 1;

      // GCD divisibility invariants.
      const T81BigInt g = T81BigInt::gcd(a, b);
      if (!T81BigInt::is_zero(g)) {
        if (!expect(T81BigInt::is_zero(T81BigInt::mod(a, g)), "gcd divisibility failed for lhs")) return 1;
        if (!expect(T81BigInt::is_zero(T81BigInt::mod(b, g)), "gcd divisibility failed for rhs")) return 1;
      }
    }
  }

  std::cout << "bigint_properties_test ok\n";
  return 0;
}
