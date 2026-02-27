# `tests/fixtures/t81lang_std_polynomial`

Fixture pack for CLI end-to-end coverage of `std.polynomial` aliases.

## Purpose
- Validate deterministic polynomial load/rewrite/canon behavior in VM execution paths.
- Validate deterministic confluence checks after rewrite and canon passes.

## Note
- Current VM polynomial confluence checks in this surface are deterministic and return `true`
  for these canonical fixture paths.
