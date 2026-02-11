# `src/bigint`

Implementation of arithmetic helpers layered on top of `T81BigInt`.

## Scope
- Euclidean division helper (`divmod`).
- Greatest-common-divisor helper (`gcd`).

## Key Files
- `divmod.cpp`: computes quotient/remainder with Euclidean remainder contract (`0 <= r < |b|`).
- `gcd.cpp`: Euclidean algorithm that uses `divmod` and returns non-negative GCD.

## Contracts
- `divmod(a, b)` requires `b != 0`.
- Remainder semantics are Euclidean, not C/C++ truncation remainder for negatives.
- `gcd(0,0)=0`, `gcd(a,0)=|a|`, `gcd(0,b)=|b|`.

## Related Headers
- `include/t81/bigint/divmod.hpp`
- `include/t81/bigint/gcd.hpp`
