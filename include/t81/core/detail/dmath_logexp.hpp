#pragma once

#include "t81/core/detail/dmath_constants.hpp"
#include "t81/core/detail/dmath_types.hpp"

namespace t81::core::detail {

template <typename T>
T exp(T x) {
  T ln3 = constants::ln3<T>();

  T div = x / ln3;

  auto round_fixed = [](T val) {
    typename T::Storage s = val.v;
    s >>= T::kFractionalTrits;
    s <<= T::kFractionalTrits;
    return T(s);
  };

  T k_approx = round_fixed(div);
  T r = x - k_approx * ln3;

  // Loop for exp(r) = 1 + r + r^2/2! + ...
  // Converges very fast. 15 terms is plenty for |r| <= 0.55
  T sum = T::one();
  T term = T::one();

  for (int n = 1; n <= 20; ++n) {
    term = term * r / T(n);
    sum = sum + term;
  }

  std::int64_t k = k_approx.to_int64();

  if (k > 0) {
    sum.v <<= static_cast<size_t>(k);
  } else if (k < 0) {
    sum.v >>= static_cast<size_t>(-k);
  }

  return sum;
}

template <typename T>
T log(T x) {
  if (x.is_negative() || x.is_zero()) {
    return T::zero();
  }

  auto& v = x.v;
  size_t lead = 0;
  for (size_t i = T::Storage::kNumTrits; i-- > 0;) {
    if (v[i] != t81::Trit::Z) {
      lead = i;
      break;
    }
  }

  std::int64_t e_eff =
      static_cast<std::int64_t>(lead) - static_cast<std::int64_t>(T::kFractionalTrits);

  std::int64_t shift =
      static_cast<std::int64_t>(T::kFractionalTrits) - static_cast<std::int64_t>(lead);

  typename T::Storage m_store = v;
  if (shift > 0)
    m_store <<= static_cast<size_t>(shift);
  else
    m_store >>= static_cast<size_t>(-shift);

  T m(m_store);

  // log(m) where m is in approx [1, 1.5] (due to MSB alignment)
  // Use series for log(1+y) where y = m-1
  // Loop 40 times for precision
  T y = m - T::one();

  T sum = T::zero();
  T neg_y = -y;

  // log(1+y) = y - y^2/2 + y^3/3 ...
  // = y + (-y)*y/2 + ...

  // Loop: term = y^n / n. Signs alternate.
  // term_1 = y. sum = y.
  // term_2 = -y^2/2.

  T num_acc = y;  // y^n

  for (int n = 1; n <= 50; ++n) {
    // term = num_acc / n
    // Sign is handled by num_acc accumulation
    // n=1: +y. num_acc=y.
    // n=2: -y^2. num_acc should become -y^2.
    // num_acc *= -y?
    // y * (-y) = -y^2.
    // -y^2 * (-y) = y^3.
    // Correct.

    T term = num_acc / T(n);
    sum = sum + term;

    num_acc = num_acc * neg_y;
  }

  T ln3 = constants::ln3<T>();
  T e_val(static_cast<int>(e_eff));

  return sum + e_val * ln3;
}

template <typename T>
T sqrt(T x) {
  if (x.is_negative()) return T::zero();
  if (x.is_zero()) return T::zero();

  T y = T::one();
  T half = constants::half<T>();

  for (int i = 0; i < 15; ++i) {
    y = half * (y + x / y);
  }

  return y;
}

template <typename T>
T pow(T x, T y) {
  if (x.is_zero()) {
    if (y.is_zero()) return T::one();  // 0^0 = 1
    if (y.is_negative())
      return T::zero();  // 0^-y -> inf (represented as max or handle in wrapper)
                         // actually wrapper handles inf. here we return 0?
                         // or just let log(0) handle it?
                         // log(0) returns 0 in current implementation?
                         // log implementation: "if (x.is_negative() || x.is_zero()) return
                         // T::zero();" So log(0) = 0. exp(y*0) = exp(0) = 1. So 0^y -> 1? That's
                         // wrong for y > 0.
    return T::zero();
  }

  if (x.is_negative()) {
    // For now return 0 (error) as we don't support complex results here yet
    return T::zero();
  }

  // x > 0
  return exp(y * log(x));
}

}  // namespace t81::core::detail
