# Chapter 8: Verification and Audit

## 8.1 Formal Verification Methodology

**Status: Process**

The T81 project employs a rigorous verification methodology to ensure that the implementation matches the specification. This is critical because the system's primary value proposition is correctness, not speed.

### 8.1.1 The Verification Pyramid

1.  **Type Checking (Static)**: The C++ compiler (Clang/GCC) ensures type safety in the host implementation. The T81Lang compiler ensures type safety in user programs.
2.  **Unit Testing (Dynamic)**: Individual components (`dmath`, `Stack`, `Memory`) are tested in isolation.
3.  **Property-Based Testing (Fuzzing)**: Generators create random inputs to verify invariants hold (e.g., $Add(A, B) == Add(B, A)$).
4.  **Integration Testing (End-to-End)**: Full programs are compiled and executed, and the resulting traces are verified.
5.  **Formal Proof (Future)**: Mathematical proofs of correctness for the core transition function.

## 8.2 The Formal Audit Matrix

The audit matrix maps every requirement in the specification to a concrete test case in the codebase.

| Requirement | Spec Section | Implementation | Verification Test |
| :--- | :--- | :--- | :--- |
| **Strict Determinism** | `2.1` | `src/vm/vm.cpp` | `tests/cpp/test_property_invariants.cpp` |
| **Ternary Arithmetic** | `4.1` | `include/t81/ternary.hpp` | `tests/cpp/ternary_arith_test.cpp` |
| **Policy Enforcement** | `1.2.2` | `src/axion/policy_engine.cpp` | `tests/cpp/test_ethics.cpp` |
| **CanonFS Hash Integrity** | `1.2.3` | `src/canonfs/` | `tests/cpp/canonfs_driver_test.cpp` |
| **Trace Equivalence (JIT)**| `3.5` | `src/vm/jit.cpp` | `tests/cpp/jit_trace_equivalence_test.cpp` |
| **Soft-Float Bit Exactness**| `4.2` | `include/t81/core/dmath.hpp` | `tests/cpp/test_T81Float.cpp` |

## 8.3 Property-Based Testing

**Status: Implemented**

T81 relies heavily on property-based testing (similar to QuickCheck or Hypothesis). instead of writing static test cases (e.g., `assert(add(1, 2) == 3)`), we define properties.

### 8.3.1 Example: Commutativity
```cpp
// Pseudocode for a property test
PROPERTY(AddIsCommutative, (T81Int a, T81Int b)) {
    auto res1 = dmath::add(a, b);
    auto res2 = dmath::add(b, a);
    ASSERT_EQ(res1, res2);
}
```
The test runner executes this property with thousands of random inputs, including edge cases like `MaxInt`, `MinInt`, and `0`.

### 8.3.2 Example: Associativity
```cpp
PROPERTY(AddIsAssociative, (T81Int a, T81Int b, T81Int c)) {
    // (a + b) + c == a + (b + c)
    // Note: This holds for Integers but NOT necessarily for Floating Point!
    // We test that it holds for T81Int, and EXPLICITLY test that it fails for T81Float
    // in expected ways (or holds if we use specific subsets).
}
```

## 8.4 The Determinism Gate (`t81lang_repro_gate`)

**Status: Critical**

The **Determinism Gate** is the final guardian of the release process. It is a script (`scripts/ci/t81lang_repro_gate.py`) that performs a "Black Box" verification of the entire toolchain.

### 8.4.1 The Protocol
1.  **Clean Build**: The script forces a clean build of the compiler and VM.
2.  **Canonical Input**: It feeds a standard reference program (`examples/reference_calc.t81`) into the compiler.
3.  **Trace Capture**: It runs the resulting bytecode and captures the Axion Trace.
4.  **Hash Comparison**: It computes the SHA3-256 hash of the trace.
5.  **Verification**: It compares this hash against a hardcoded "Golden Hash".

### 8.4.2 Failure Implication
If the hash differs even by one bit, the build fails. This indicates that a change in the code (e.g., a library update, a compiler flag change, or a logic error) has altered the deterministic behavior of the machine. The PR cannot be merged until the issue is resolved or, if the change is intentional (e.g., a bug fix in `sin()`), the Golden Hash is updated with a detailed audit note explaining the divergence.
