# Chapter 7: Verification and Audit

## 7.1 The Verification Stack

**Status: Implemented**

T81 provides a comprehensive suite of verification tools to ensure correctness at every level of the stack.

1.  **Unit Tests**: Low-level C++ tests in `tests/cpp/`.
2.  **Integration Tests**: End-to-end T81Lang programs in `tests/fixtures/`.
3.  **Property Tests**: Randomized property-based testing for arithmetic invariants (`tests/cpp/test_property_invariants.cpp`).
4.  **Determinism Gate**: CI script `scripts/ci/t81lang_repro_gate.py` that enforces bit-exact reproducibility.

## 7.2 Determinism Gate

**Status: Implemented & Active**

The `t81lang_repro_gate.py` script is the primary arbiter of build correctness. It:
1.  Compiles a standard test suite (`tests/fixtures/t81lang_determinism`).
2.  Computes the SHA-256 hash of the generated TISC bytecode.
3.  Compares these hashes against a canonical manifest (`repro.json`).

If any hash differs, the build fails. This ensures that compiler changes do not inadvertently alter code generation.

## 7.3 Trace Verification

**Status: Implemented**

The Axion Trace allows for post-hoc verification of execution.
*   **Replay**: `t81 trace replay program.tisc trace.txt` re-executes the program and verifies that the recorded trace matches the live execution.
*   **Diff**: `t81 trace diff trace_a.txt trace_b.txt` highlights divergences between two runs.

> **Verification**: `tests/cpp/axion_log_determinism_test.cpp` ensures trace integrity.
