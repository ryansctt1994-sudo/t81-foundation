# `tests/fixtures/t81lang_std_text`

Fixture pack for CLI end-to-end coverage of `std.text` builtins.

## Purpose
- Validate deterministic observable output for `std.text` operations when executed via `t81 run`.
- Current fixture set covers `str_len`, `str_is_empty`, `concat`, `starts_with`, `ends_with`,
  `contains`, `index_of`, `replace`, `to_string`, and `from_bytes`.

## Expectations
- Keep fixtures small and behavior-focused.
- Every `.t81` fixture has a matching `.out` golden file.
