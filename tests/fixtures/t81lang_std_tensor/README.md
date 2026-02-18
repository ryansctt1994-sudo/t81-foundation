# `tests/fixtures/t81lang_std_tensor`

Fixture pack for CLI end-to-end coverage of `std.tensor` aliases.

## Purpose
- Validate deterministic observable output for literal-path `std.tensor.load`.
- Exercise `std.tensor.from_list` and `std.tensor.vec_add` in runtime execution paths.
- Exercise `std.tensor.matmul` in runtime execution paths using deterministic rank-2 fixture weights.

## Note
- `std.tensor.load` currently requires a string literal argument at the callsite.
- The fixture runner injects an in-memory deterministic weights model (`mat_a`, `mat_b`) so
  `std.tensor.matmul` can execute on rank-2 tensors without external files.
