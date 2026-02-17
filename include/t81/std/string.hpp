/**
 * @file string.hpp
 * @brief Standard string utilities and conversions.
 */
#pragma once

#include <iomanip>
#include <sstream>
#include <vector>
#include "t81/core/T81Float.hpp"
#include "t81/core/T81Int.hpp"
#include "t81/core/T81String.hpp"

namespace t81::text {

using String = t81::T81String;

// String manipulation
inline ::std::vector<String> split(const String& s, char delimiter) {
  ::std::vector<String> result;
  ::std::string_view sv = s.sv();
  size_t start = 0;
  size_t end = sv.find(delimiter);
  while (end != ::std::string_view::npos) {
    result.emplace_back(sv.substr(start, end - start));
    start = end + 1;
    end = sv.find(delimiter, start);
  }
  result.emplace_back(sv.substr(start));
  return result;
}

inline String join(const ::std::vector<String>& parts, const String& delimiter) {
  if (parts.empty()) return String("");
  String result = parts[0];
  for (size_t i = 1; i < parts.size(); ++i) {
    result += delimiter;
    result += parts[i];
  }
  return result;
}

// Conversions
template <typename T>
String to_string(const T& val) {
  // Fallback to stream operator
  ::std::ostringstream oss;
  oss << val;
  return String(::std::string_view(oss.str()));
}

// Specializations/Overloads
template <size_t N>
String to_string(const T81Int<N>& val) {
  return String(::std::string_view(val.to_string()));
}

// Float formatting (native if possible, else double)
template <size_t M, size_t E>
String to_string(const T81Float<M, E>& val) {
  // For now, rely on double conversion for decimal output
  // TODO: Implement native ternary-to-decimal conversion
  ::std::ostringstream oss;
  if (val.is_nae()) return String("NAE");
  if (val.is_inf()) return String(val.is_negative() ? "-INF" : "INF");

  // We use high precision
  oss << ::std::setprecision(15) << val.to_double();
  return String(::std::string_view(oss.str()));
}

}  // namespace t81::text
