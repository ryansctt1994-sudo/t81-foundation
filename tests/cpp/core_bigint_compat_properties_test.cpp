#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <random>

#include "t81/core/bigint.hpp"
#include "t81/bigint.hpp"

int main() {
  using t81::T81BigInt;
  using t81::core::BigInt;

  const auto require = [](bool cond) {
    if (!cond) {
      std::cerr << "core_bigint_compat_properties_test invariant failed\n";
      std::abort();
    }
  };

  // Deterministic edge corpus for constructor/to_int64/to_string compatibility.
  constexpr std::int64_t kEdges[] = {
      std::numeric_limits<std::int64_t>::min(),
      std::numeric_limits<std::int64_t>::max(),
      0,
      1,
      -1,
      81,
      -81,
      243,
      -243,
      729,
      -729,
      59049,
      -59049,
  };

  for (const std::int64_t v : kEdges) {
    const BigInt b(v);
    require(b.value() == v);
    require(b.canonical().to_int64() == v);
    require(b.to_string() == T81BigInt::from_i64(v).to_string());
  }

  // Deterministic randomized coverage for representative 64-bit values.
  std::mt19937_64 rng(0xB16B00B5ULL);
  std::uniform_int_distribution<std::int64_t> dist(
      std::numeric_limits<std::int64_t>::min(),
      std::numeric_limits<std::int64_t>::max());

  for (int i = 0; i < 5000; ++i) {
    const std::int64_t v = dist(rng);
    const BigInt b(v);
    require(b.value() == v);
    require(b.canonical().to_int64() == v);
    require(b.to_string() == T81BigInt::from_i64(v).to_string());
  }

  std::cout << "core_bigint_compat_properties_test ok\n";
  return 0;
}
