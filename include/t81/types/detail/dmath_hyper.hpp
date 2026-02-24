#pragma once

#include "t81/types/detail/dmath_constants.hpp"
#include "t81/types/detail/dmath_logexp.hpp"
#include "t81/types/detail/dmath_types.hpp"

namespace t81::core::detail {

template <typename T>
T sinh(T x) {
  // sinh(x) = (exp(x) - exp(-x)) / 2
  T e = exp(x);
  T em = exp(-x);
  return (e - em) * constants::half<T>();
}

template <typename T>
T cosh(T x) {
  // cosh(x) = (exp(x) + exp(-x)) / 2
  T e = exp(x);
  T em = exp(-x);
  return (e + em) * constants::half<T>();
}

template <typename T>
T tanh(T x) {
  // tanh(x) = sinh(x) / cosh(x)
  // or (e - em) / (e + em)
  T e = exp(x);
  T em = exp(-x);
  T num = e - em;
  T den = e + em;
  if (den.is_zero()) {
    return T::zero();  // Should be inf but DFixed has limits
  }
  return num / den;
}

}  // namespace t81::core::detail
