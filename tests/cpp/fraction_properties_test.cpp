#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>

#include "t81/fraction.hpp"

int main() {
  using t81::T81BigInt;
  using t81::T81Fraction;

  const auto require = [](bool cond) {
    if (!cond) {
      std::cerr << "fraction_properties_test invariant failed\n";
      std::abort();
    }
  };

  std::mt19937_64 rng(0xF81AC710ULL);
  std::uniform_int_distribution<std::int64_t> num_dist(-10000, 10000);
  std::uniform_int_distribution<std::int64_t> den_dist(-10000, 10000);

  const T81Fraction zero = T81Fraction::from_int(0);
  const T81Fraction one = T81Fraction::from_int(1);

  for (int i = 0; i < 2000; ++i) {
    const std::int64_t n1 = num_dist(rng);
    std::int64_t d1 = den_dist(rng);
    while (d1 == 0) d1 = den_dist(rng);

    const std::int64_t n2 = num_dist(rng);
    std::int64_t d2 = den_dist(rng);
    while (d2 == 0) d2 = den_dist(rng);

    const T81Fraction a(T81BigInt::from_i64(n1), T81BigInt::from_i64(d1));
    const T81Fraction b(T81BigInt::from_i64(n2), T81BigInt::from_i64(d2));

    // Denominator canonicalization invariant: denominator > 0.
    require(!T81BigInt::is_neg(a.den));
    require(!T81BigInt::is_neg(b.den));

    // Add/sub inverse law: (a + b) - b == a
    require(T81Fraction::cmp(T81Fraction::sub(T81Fraction::add(a, b), b), a) == 0);

    // Multiplicative identity.
    require(T81Fraction::cmp(T81Fraction::mul(a, one), a) == 0);

    // Additive identity.
    require(T81Fraction::cmp(T81Fraction::add(a, zero), a) == 0);

    // Negation involution.
    require(T81Fraction::cmp(T81Fraction::neg(T81Fraction::neg(a)), a) == 0);

    if (!T81BigInt::is_zero(b.num)) {
      // Division inverse: (a / b) * b == a
      const T81Fraction round = T81Fraction::mul(T81Fraction::div(a, b), b);
      require(T81Fraction::cmp(round, a) == 0);
    }

    // Ordering antisymmetry via cmp.
    const int ab = T81Fraction::cmp(a, b);
    const int ba = T81Fraction::cmp(b, a);
    require((ab == 0 && ba == 0) || (ab < 0 && ba > 0) || (ab > 0 && ba < 0));
  }

  std::cout << "fraction_properties_test ok\n";
  return 0;
}
