# T81Lang Standard Library Promotion Snapshot

Date: 2026-02-26  
Status: Active Snapshot  
Baseline: `c5c4aa59`

## 1. Module Status Matrix

| Module | Status | Determinism Posture | Evidence |
| :--- | :--- | :--- | :--- |
| `std.core` | bounded | deterministic aliases; behavior locked by fixtures | `tests/cpp/cli_std_core_fixtures_test.cpp` |
| `std.math` | bounded | host-math dependency documented; bounded deterministic profile only | `tests/cpp/cli_std_math_fixtures_test.cpp`, `spec/tisc-spec.md` |
| `std.io` | bounded | deterministic handle aliases and print paths | `tests/cpp/cli_std_runtime_fixtures_test.cpp` |
| `std.collections` | stable | deterministic staged semantics and fixture coverage | `tests/cpp/cli_std_collections_fixtures_test.cpp` |
| `std.text` | stable | deterministic text semantics and fixture coverage | `tests/cpp/cli_std_text_fixtures_test.cpp` |
| `std.bytes` | stable | deterministic byte semantics and fixture coverage | `tests/cpp/cli_std_bytes_fixtures_test.cpp` |
| `std.symbol` | stable | deterministic alias semantics and fixture coverage | `tests/cpp/cli_std_symbol_fixtures_test.cpp` |
| `std.sys` | bounded | deterministic placeholder aliases (`time=0`, `entropy=0`) | `tests/cpp/cli_std_runtime_fixtures_test.cpp` |
| `std.async` | bounded | deterministic placeholder aliases (`yield/sleep` no-op) | `tests/cpp/cli_std_runtime_fixtures_test.cpp` |
| `std.tensor` | bounded | deterministic alias behavior within current runtime profile | `tests/cpp/cli_std_tensor_fixtures_test.cpp` |
| `std.agent` | experimental | deterministic opcode alias exists; capability semantics remain bounded | `tests/cpp/cli_std_runtime_fixtures_test.cpp` |

## 2. Governance Notes

1. This snapshot is descriptive, not a global determinism claim expansion.
2. Any status promotion/demotion requires:
   - snapshot update,
   - evidence update,
   - release/status synchronization.
3. `std.math` remains bounded until host-dependent transcendental surfaces are
   fully contract-resolved for cross-platform bit-identity claims.

