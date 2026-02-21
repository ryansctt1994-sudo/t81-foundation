# Chapter 7: Testing and Benchmarks

Reliability and determinism are the cornerstones of T81. The project includes a multi-layered testing strategy to ensure these properties are never compromised.

## 7.1 Test Suites

Tests are located in `tests/cpp/` and are managed by CTest. They cover:

*   **Unit Tests**: Verify individual components (e.g., `t81_bigint_test` checks arithmetic correctness).
*   **Property Tests**: Use randomized inputs to verify mathematical invariants (e.g., `a + b == b + a`).
*   **Integration Tests**: Compile and run full T81Lang programs to verify the entire pipeline (`e2e_arithmetic_test`).
*   **Fuzz Tests**: Feed random garbage to the parser and VM to ensure they fail gracefully and deterministically, without crashing or undefined behavior.

### Running Tests
```bash
cd build
ctest
```

## 7.2 Benchmarks

Performance benchmarks are located in `benchmarks/`. They measure the throughput of:
*   T81BigInt operations (Add, Mul, Div).
*   Tensor operations (MatMul, Conv).
*   VM instruction dispatch overhead.

### Running Benchmarks
```bash
./build/t81 benchmark
```
This invokes the `benchmark_runner`, which executes defined scenarios and reports timing metrics.

**Note**: Because T81 uses software-defined math and strict safety checks, it is expected to be slower than native C++ `int64_t` arithmetic. The goal is *efficiency within the constraints of determinism*, not beating hardware ALUs.

## 7.3 Reproducibility Gates

The most critical test in the CI pipeline is the **Determinism Gate** (`scripts/ci/t81lang_repro_gate.py`).

### How it Works
1.  **Fixtures**: A set of T81Lang programs in `tests/fixtures/t81lang_determinism`.
2.  **Pass A**: The script compiles all fixtures to TISC (`.tisc`) files.
3.  **Pass B**: The script immediately recompiles all fixtures again.
4.  **Comparison**: It compares the bitwise SHA-256 hash of Pass A output vs. Pass B output.
5.  **Golden Master**: It compares the resulting hash against a "known good" hash committed in the repo.

If any byte differs—due to uninitialized memory, pointer-based sorting, or time-dependent logic in the compiler—the gate fails. This ensures the toolchain itself is deterministic.

## 7.4 Fuzzing

Frontend fuzzing (`t81_frontend_fuzz_test`) is crucial for security. It generates random byte streams and feeds them to the T81Lang lexer and parser. The requirement is that the compiler must either:
1.  Successfully compile valid code.
2.  Reject invalid code with a deterministic error message.
3.  **Never** crash (segfault) or hang.
