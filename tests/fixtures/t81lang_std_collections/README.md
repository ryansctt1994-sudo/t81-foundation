# `tests/fixtures/t81lang_std_collections`

Fixture pack for CLI end-to-end coverage of `std.collections` aliases.

## Purpose
- Validate deterministic observable behavior for `std.collections.len`,
  `std.collections.is_empty`, `std.collections.first`, `std.collections.last`,
  `std.collections.push`, and `std.collections.pop`.
- Validate behavior for both frontend `Vector[i32]` values and
  `Vector[T81String]` values produced via `std.text.split`.
- Validate end-to-end generic call inference over nested container signatures
  (for example `Vector[T]` and `Option[T]`) through fixture execution and
  golden outputs.
