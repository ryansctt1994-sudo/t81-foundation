#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "t81/bigint.hpp"
#include "t81/bigint/divmod.hpp"

int main() {
  using t81::T81BigInt;

  const auto check_pair = [&](std::int64_t ai, std::int64_t bi) {
    if (bi == 0) return;

    const T81BigInt a = T81BigInt::from_i64(ai);
    const T81BigInt b = T81BigInt::from_i64(bi);
    const auto dm = t81::divmod(a, b);

    // Euclidean recomposition law.
    const T81BigInt recomposed = T81BigInt::add(T81BigInt::mul(dm.q, b), dm.r);
    if (!(recomposed == a)) {
      std::cerr << "recompose failed ai=" << ai << " bi=" << bi
                << " q=" << dm.q.to_string() << " r=" << dm.r.to_string()
                << " recomposed=" << recomposed.to_string()
                << " a=" << a.to_string() << "\n";
      std::abort();
    }

    // Euclidean remainder range: 0 <= r < |b|.
    if (T81BigInt::is_neg(dm.r)) {
      std::cerr << "negative remainder ai=" << ai << " bi=" << bi
                << " q=" << dm.q.to_string() << " r=" << dm.r.to_string() << "\n";
      std::abort();
    }
    if (!(T81BigInt::cmp(dm.r, T81BigInt::abs(b)) < 0)) {
      std::cerr << "remainder bound failed ai=" << ai << " bi=" << bi
                << " q=" << dm.q.to_string() << " r=" << dm.r.to_string()
                << " |b|=" << T81BigInt::abs(b).to_string() << "\n";
      std::abort();
    }

    // Division API consistency:
    // - if remainder is zero, div must return the Euclidean quotient.
    // - otherwise div may throw OR (legacy behavior) return zero when |a| < |b|.
    bool div_threw = false;
    T81BigInt div_q = T81BigInt::zero();
    try {
      div_q = T81BigInt::div(a, b);
    } catch (const std::domain_error&) {
      div_threw = true;
    }

    if (T81BigInt::is_zero(dm.r)) {
      if (div_threw || !(div_q == dm.q)) {
        std::cerr << "exact division mismatch ai=" << ai << " bi=" << bi
                  << " q(divmod)=" << dm.q.to_string()
                  << " q(div)=" << div_q.to_string()
                  << " threw=" << (div_threw ? "true" : "false") << "\n";
        std::abort();
      }
    } else if (!div_threw) {
      const bool legacy_small_division =
          T81BigInt::is_zero(div_q) &&
          T81BigInt::cmp(T81BigInt::abs(a), T81BigInt::abs(b)) < 0;
      if (!legacy_small_division) {
        std::cerr << "non-exact division accepted unexpectedly ai=" << ai << " bi=" << bi
                  << " q(divmod)=" << dm.q.to_string()
                  << " r(divmod)=" << dm.r.to_string()
                  << " q(div)=" << div_q.to_string() << "\n";
        std::abort();
      }
    }
  };

  const std::vector<std::int64_t> edge_values = {
      std::numeric_limits<std::int64_t>::min(),
      std::numeric_limits<std::int64_t>::min() + 1,
      -1'000'000'000'000LL,
      -6561, -243, -82, -81, -80, -2, -1, 0, 1, 2, 80, 81, 82, 243, 6561,
      1'000'000'000'000LL,
      std::numeric_limits<std::int64_t>::max() - 1,
      std::numeric_limits<std::int64_t>::max(),
  };

  for (const std::int64_t ai : edge_values) {
    for (const std::int64_t bi : edge_values) {
      check_pair(ai, bi);
    }
  }

  // Deterministic random stress around typical value ranges.
  std::mt19937_64 rng(0xD1A81DEDULL);
  std::uniform_int_distribution<std::int64_t> dist(-2'000'000'000LL, 2'000'000'000LL);
  for (int i = 0; i < 5000; ++i) {
    const std::int64_t ai = dist(rng);
    std::int64_t bi = dist(rng);
    while (bi == 0) bi = dist(rng);
    check_pair(ai, bi);
  }

  // Fast path sanity for unit divisors.
  for (const std::int64_t ai : edge_values) {
    const T81BigInt a = T81BigInt::from_i64(ai);
    const auto dm_pos = t81::divmod(a, T81BigInt::one());
    if (!(dm_pos.q == a) || !T81BigInt::is_zero(dm_pos.r)) {
      std::cerr << "unit +1 failed ai=" << ai << " q=" << dm_pos.q.to_string()
                << " r=" << dm_pos.r.to_string() << " a=" << a.to_string() << "\n";
      std::abort();
    }

    const auto dm_neg = t81::divmod(a, T81BigInt::from_i64(-1));
    if (!(dm_neg.q == T81BigInt::neg(a)) || !T81BigInt::is_zero(dm_neg.r)) {
      std::cerr << "unit -1 failed ai=" << ai << " q=" << dm_neg.q.to_string()
                << " expected=" << T81BigInt::neg(a).to_string()
                << " r=" << dm_neg.r.to_string() << "\n";
      std::abort();
    }
  }

  // Large-magnitude stress for one-limb divisors (fast path in divmod_nonneg_).
  auto make_large_base81_digits = [](std::mt19937_64& gen, std::size_t n) {
    std::uniform_int_distribution<int> digit_dist(0, 80);
    std::string s;
    s.reserve(n * 3);
    int first = digit_dist(gen);
    while (first == 0) first = digit_dist(gen);
    s += std::to_string(first);
    for (std::size_t i = 1; i < n; ++i) {
      s.push_back('.');
      s += std::to_string(digit_dist(gen));
    }
    return s;
  };

  std::mt19937_64 large_rng(0xF81A57DULL);
  const T81BigInt large =
      T81BigInt::from_base81_digit_string(make_large_base81_digits(large_rng, 320));
  const std::vector<int> small_divisors = {2, 3, 5, 7, 9, 27, 80};
  for (const int di : small_divisors) {
    const T81BigInt b = T81BigInt::from_i64(di);
    const auto dm = t81::divmod(large, b);
    const T81BigInt recomposed = T81BigInt::add(T81BigInt::mul(dm.q, b), dm.r);
    if (!(recomposed == large)) {
      std::cerr << "large one-limb recompose failed d=" << di << "\n";
      std::abort();
    }
    if (T81BigInt::is_neg(dm.r) || !(T81BigInt::cmp(dm.r, T81BigInt::abs(b)) < 0)) {
      std::cerr << "large one-limb remainder bounds failed d=" << di << "\n";
      std::abort();
    }
  }

  std::cout << "bigint_division_edge_properties_test ok\n";
  return 0;
}
