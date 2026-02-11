# tests

Regression and conformance proof layer for T81.

## Layout
- `tests/cpp/`: primary C++ suite (VM, frontend, TISC, Axion, CLI, numerics)
- `tests/ci/`: unit tests for CI policy scripts
- `tests/fixtures/`: deterministic fixture inputs (including repro-hash fixtures)
- `tests/harness/`: auxiliary canonical vectors and harness assets
- `tests/python/`: Python-side checks where applicable

## Expectations
- Every behavior change in `src/` or `include/t81/` should include tests.
- Determinism-sensitive changes should include reproducibility coverage.
- Prefer explicit runtime checks over raw `assert` in standalone test binaries.

## Common runs
```bash
ctest --test-dir build --output-on-failure
ctest --test-dir build --output-on-failure -R "(axion|jit|determinism|cli)"
```
