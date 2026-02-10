#pragma once

#include "t81/core/T81BigInt.hpp"
#include "t81/core/T81Float.hpp"
#include "t81/core/T81Fraction.hpp"

#include <limits>
#include <locale>
#include <sstream>
#include <string>

namespace t81::lang {

inline std::string format_canonical(const t81::v1::T81BigInt& value) {
  return value.str() + "t81";
}

template <std::size_t M, std::size_t E>
std::string format_canonical(const t81::v1::T81Float<M, E>& value) {
  double numeric = value.to_double();
  if (numeric == 0.0) {
    numeric = 0.0;
  }
  std::ostringstream out;
  out.imbue(std::locale::classic());
  out.precision(std::numeric_limits<double>::max_digits10);
  out << numeric << "t81";
  return out.str();
}

template <std::size_t N>
std::string format_canonical(const t81::v1::T81Fraction<N>& value) {
  return value.num().to_trit_string() + "/" + value.den().to_trit_string() + "t81";
}

}  // namespace t81::lang
