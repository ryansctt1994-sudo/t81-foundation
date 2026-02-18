# t81lang std.runtime fixtures

This fixture set validates deterministic runtime behavior for staged `std.sys` / `std.io` /
`std.async` aliases.

## Covered behavior

- `std.sys.proof`, `std.io.stream`, `std.io.net`, `std.async.thread`, and `std.async.promise`
  print stable symbolic token values.
- `std.sys.time` and `std.sys.entropy` retain deterministic baseline behavior (`0.0` and `0`),
  and `std.async.yield` / `std.async.sleep` are deterministic no-op aliases.
