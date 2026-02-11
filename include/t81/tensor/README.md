# `include/t81/tensor`

Public tensor operation interfaces.

## Key Headers
- `ops.hpp`: aggregate include for tensor operations.
- `transpose.hpp`: transpose operations.
- `slice.hpp`: slicing operations.
- `reshape.hpp`: reshape operations.

## Design Notes
- Headers are intentionally lightweight and dependency-minimal.
- API should remain backend-agnostic so optimized implementations can swap in behind stable signatures.

## Testing
- Validate shape invariants, bounds behavior, and determinism in `tests/cpp`.
