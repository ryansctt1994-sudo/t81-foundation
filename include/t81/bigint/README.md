# `include/t81/bigint`

Public BigInt helper APIs for higher-level arithmetic routines.

## Key Headers
- `divmod.hpp`: Euclidean quotient/remainder API returning `DivModResult`.
- `gcd.hpp`: non-negative GCD API over `T81BigInt`.

## API Contracts
- `divmod` enforces Euclidean remainder bounds.
- `gcd` always returns a non-negative value.

## Consumers
- Implementations in `src/bigint`.
- Any runtime/tooling code requiring Euclidean integer arithmetic.
