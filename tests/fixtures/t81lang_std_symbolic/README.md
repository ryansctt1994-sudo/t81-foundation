# `tests/fixtures/t81lang_std_symbolic`

Fixture pack for CLI end-to-end coverage of `std.symbolic` aliases.

## Purpose
- Validate deterministic symbolic load/rewrite/canon behavior in VM execution paths.
- Validate deterministic confluence checks after rewrite and canon passes.

## Note
- Current VM symbolic confluence checks in this surface are deterministic and return `true`
  for these canonical fixture paths.
