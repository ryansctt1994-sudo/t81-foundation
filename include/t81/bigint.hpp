/**
 * @file bigint.hpp
 * @brief Legacy header providing T81BigInt via the robust implementation.
 *
 * This file is a compatibility shim that aliases `t81::T81BigInt` to the
 * canonical `t81::v1::T81BigInt` implementation in `t81/types/T81BigInt.hpp`.
 */
#pragma once
#include "t81/types/T81BigInt.hpp"

namespace t81 {

using T81BigInt = t81::v1::T81BigInt;
using BigInt = T81BigInt;
using T243BigInt = T81BigInt;  // Legacy alias

struct DivModResult {
  T81BigInt q;
  T81BigInt r;
};

inline DivModResult divmod(const T81BigInt& a, const T81BigInt& b) {
  auto p = T81BigInt::div_mod(a, b);
  return {p.first, p.second};
}

inline T81BigInt gcd(const T81BigInt& a, const T81BigInt& b) { return T81BigInt::gcd(a, b); }

}  // namespace t81

namespace t81::v1 {
using CanonicalBigInt = ::t81::T81BigInt;
}
