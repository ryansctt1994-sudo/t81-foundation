#pragma once

#include <utility>
#include "t81/core/detail/dmath_constants.hpp"
#include "t81/core/detail/dmath_logexp.hpp"
#include "t81/core/detail/dmath_types.hpp"

namespace t81::core::detail {

template <typename T>
std::pair<T, T> sin_cos_reduced(T x) {
  // x is in [-pi/4, pi/4]
  T x2 = x * x;

  // Cosine
  T c = T::one();
  c = T::one() -
      x2 * (constants::half<T>() -
            x2 * (constants::one_24th<T>() -
                  x2 * (constants::one_720th<T>() - x2 * (constants::one_40320th<T>() -
                                                          x2 * (constants::one_3628800th<T>())))));

  // Sine
  T s_poly =
      T::one() -
      x2 * (constants::one_sixth<T>() -
            x2 * (constants::one_120th<T>() - x2 * (constants::one_5040th<T>() -
                                                    x2 * (constants::one_362880th<T>() -
                                                          x2 * (constants::one_39916800th<T>())))));

  T s = x * s_poly;

  return {s, c};
}

template <typename T>
T sin(T x) {
  // Range reduction
  T pi_2 = constants::pi_2<T>();

  T div = x / pi_2;

  auto round_fixed = [](T val) {
    typename T::Storage s = val.v;
    s >>= T::kFractionalTrits;
    s <<= T::kFractionalTrits;
    return T(s);
  };

  T k_approx = round_fixed(div);
  T r = x - k_approx * pi_2;

  std::int64_t k_int = k_approx.to_int64();
  std::int64_t q = k_int % 4;
  if (q < 0) q += 4;

  auto [s, c] = sin_cos_reduced(r);

  switch (q) {
    case 0:
      return s;
    case 1:
      return c;
    case 2:
      return -s;
    case 3:
      return -c;
  }
  return T::zero();
}

template <typename T>
T cos(T x) {
  return sin(x + constants::pi_2<T>());
}

template <typename T>
T tan(T x) {
  return sin(x) / cos(x);
}

template <typename T>
T atan(T x) {
  if (x.is_negative()) return -atan(-x);

  if (x > T::one()) {
    return constants::pi_2<T>() - atan(T::one() / x);
  }

  // 0 <= x <= 1
  // Argument reduction: if x > sqrt(2)-1 (~0.414), use atan(x) = pi/4 + atan((x-1)/(1+x))
  // sqrt(2) - 1 = 0.4142135623730950488
  T limit = T::from_decimal(0, 4142135623730950488ULL, 19);

  if (x > limit) {
    T num = x - T::one();
    T den = T::one() + x;
    return constants::pi_4<T>() + atan(num / den);
  }

  // Series expansion for small x
  T x2 = x * x;
  T sum = x;
  T x_pow = x;

  // 15 terms is sufficient for |x| <= 0.414
  for (int n = 1; n <= 15; ++n) {
    x_pow = x_pow * x2;
    T term = x_pow / T(2 * n + 1);
    if (n % 2 == 1) {
      sum = sum - term;
    } else {
      sum = sum + term;
    }
  }
  return sum;
}

template <typename T>
T asin(T x) {
  if (x > T::one() || x < -T::one()) return T::zero();  // Domain error
  if (x.is_zero()) return T::zero();
  if (x == T::one()) return constants::pi_2<T>();
  if (x == -T::one()) return -constants::pi_2<T>();

  T x2 = x * x;
  T omx2 = T::one() - x2;
  T sq = sqrt(omx2);
  return atan(x / sq);
}

template <typename T>
T acos(T x) {
  return constants::pi_2<T>() - asin(x);
}

}  // namespace t81::core::detail
