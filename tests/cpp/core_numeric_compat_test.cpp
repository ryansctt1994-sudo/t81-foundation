#include <cassert>
#include <iostream>

#include "t81/types/bigint.hpp"
#include "t81/types/fraction.hpp"
#include "t81/fraction.hpp"

int main() {
  using t81::core::BigInt;
  using t81::core::Fraction;

  const BigInt a(123);
  assert(a.value() == 123);
  assert(a.canonical().to_int64() == 123);

  const BigInt b(77);
  assert(BigInt::add(a, b).to_string() == "2.38");
  assert(BigInt::sub(a, b).value() == 46);
  assert(BigInt::mul(BigInt(9), BigInt(9)).value() == 81);

  Fraction reduced{BigInt(2), BigInt(4)};
  assert(reduced.to_string() == "1/2");
  assert(reduced.is_canonicalizable());
  assert(reduced.canonical().to_string() == "1/2");

  Fraction sign_norm{BigInt(-3), BigInt(-9)};
  assert(sign_norm.to_string() == "1/3");

  // Legacy fallback path: denominator zero remains representable as text.
  Fraction invalid{BigInt(7), BigInt(0)};
  assert(invalid.to_string() == "7/0");
  assert(!invalid.is_canonicalizable());

  const t81::T81Fraction canonical_half(t81::T81BigInt::from_i64(5), t81::T81BigInt::from_i64(10));
  const Fraction via_ctor(canonical_half);
  assert(via_ctor.to_string() == "1/2");

  std::cout << "core_numeric_compat_test ok\n";
  return 0;
}
