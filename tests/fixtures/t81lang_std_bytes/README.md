# `tests/fixtures/t81lang_std_bytes`

Fixture pack for CLI end-to-end coverage of `std.bytes` aliases.

## Purpose
- Validate deterministic observable output for `std.bytes` operations.
- Current fixture set covers `len`, `is_empty`, `concat`, `starts_with`, `ends_with`,
  `contains`, `index_of`, `replace`, and `to_string`.

## Note
- Fixtures construct explicit `T81Bytes` values via `T81Bytes("...")` to validate
  byte-typed frontend semantics.
