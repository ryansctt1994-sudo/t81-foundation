#include "t81/lang/numeric_format.hpp"

#include <cassert>
#include <iostream>

int main() {
  using t81::lang::format_canonical;
  using t81::v1::T81BigInt;
  using t81::v1::T81Float;
  using t81::v1::T81Fraction;

  const T81BigInt bigint(42);
  const std::string bigint_s = format_canonical(bigint);
  assert(bigint_s == format_canonical(bigint));
  assert(bigint_s.size() >= 3);
  assert(bigint_s.substr(bigint_s.size() - 3) == "t81");

  const auto f = T81Float<72, 9>::from_double(1.25);
  const std::string float_s = format_canonical(f);
  assert(float_s == format_canonical(f));
  assert(float_s.size() >= 3);
  assert(float_s.substr(float_s.size() - 3) == "t81");

  const T81Fraction<81> q(T81Fraction<81>::Int(22), T81Fraction<81>::Int(7));
  const std::string frac_s = format_canonical(q);
  assert(frac_s == format_canonical(q));
  assert(frac_s.find('/') != std::string::npos);
  assert(frac_s.size() >= 3);
  assert(frac_s.substr(frac_s.size() - 3) == "t81");

  std::cout << "T81 numeric formatting tests passed!\n";
  return 0;
}
