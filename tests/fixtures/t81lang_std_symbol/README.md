# `tests/fixtures/t81lang_std_symbol`

Fixture pack for CLI end-to-end coverage of `std.symbol` aliases.

## Purpose
- Validate deterministic observable output for `std.symbol.intern` and
  `std.symbol.to_string`.

## Note
- T81Lang currently represents symbol values as interned `T81String` handles,
  so fixtures assert deterministic string-visible behavior.
