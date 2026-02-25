#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>

#include "t81/fraction.hpp"
#include "t81/types/fraction.hpp"

int main() {
  using t81::T81BigInt;
  using t81::T81Fraction;
  using t81::core::BigInt;
  using t81::core::Fraction;

  const auto require = [](bool cond) {
    if (!cond) {
      std::cerr << "core_fraction_compat_properties_test invariant failed\n";
      std::abort();
    }
  };

  std::mt19937_64 rng(0xC0FFEE1234ULL);
  std::uniform_int_distribution<std::int64_t> num_dist(-20000, 20000);
  std::uniform_int_distribution<std::int64_t> den_dist(-20000, 20000);

  for (int i = 0; i < 2500; ++i) {
    const std::int64_t n = num_dist(rng);
    std::int64_t d = den_dist(rng);
    while (d == 0) d = den_dist(rng);

    const Fraction core_fraction{BigInt(n), BigInt(d)};
    const std::string got = core_fraction.to_string();
    const std::string expected =
        T81Fraction(T81BigInt::from_i64(n), T81BigInt::from_i64(d)).to_string();
    require(got == expected);
  }

  for (int i = 0; i < 400; ++i) {
    const std::int64_t n = num_dist(rng);
    const Fraction legacy{BigInt(n), BigInt(0)};
    const std::string expected = T81BigInt::from_i64(n).to_string() + "/0";
    require(legacy.to_string() == expected);
  }

  std::cout << "core_fraction_compat_properties_test ok\n";
  return 0;
}
