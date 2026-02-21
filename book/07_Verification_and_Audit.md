# Chapter 7: Verification and Audit

## 7.1 Formal Verification Methodology

The T81 Foundation employs a multi-layered verification strategy to ensure that the implementation adheres to the formal specification. This chapter serves as the **Audit Handbook** for certifying a T81VM implementation.

### 7.1.1 Layers of Assurance

1.  **Unit Tests (`ctest`)**: Verify individual function correctness (e.g., `Add` opcode, `T81Float::normalize`).
2.  **Property-Based Tests (`fuzz`)**: Verify mathematical invariants (e.g., `(a + b) == (b + a)`) across millions of random inputs.
3.  **Determinism Gate (`repro_gate`)**: Verify end-to-end reproducibility of the compiler and VM.
4.  **Trace Audit**: Verify that the Axion execution log matches the canonical reference trace bit-for-bit.

## 7.2 The Formal Audit Matrix

This matrix maps each high-level requirement to its specific verification artifact in the codebase.

| Requirement ID | Description | Spec Section | Implementation | Verification Test | CI Coverage | Determinism Tier |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **REQ-VM-01** | Strict Determinism (x86/ARM) | `spec/determinism-profile.md` | `src/vm/vm.cpp` | `scripts/ci/t81lang_repro_gate.py` | 100% | Tier A |
| **REQ-VM-02** | Ternary Arithmetic Correctness | `spec/t81-data-types.md` | `include/t81/ternary.hpp` | `tests/cpp/test_ternary_math.cpp` | 100% | Tier A |
| **REQ-VM-03** | Axion Policy Enforcement | `spec/axion-kernel.md` | `src/axion/policy_engine.cpp` | `tests/cpp/test_ethics.cpp` | 100% | Tier B |
| **REQ-VM-04** | Recursion Limit (Stack/Depth) | `spec/cognitive-tiers.md` | `src/vm/vm.cpp` | `tests/cpp/test_tier3_opcodes.cpp` | 100% | Tier B |
| **REQ-VM-05** | Float Canonicalization | `spec/t81-data-types.md` | `include/t81/core/T81Float.hpp` | `tests/cpp/test_property_float.cpp` | 95% | Tier C (dmath) |
| **REQ-VM-06** | Tensor Quantization (T3_K) | `spec/t81-data-types.md` | `include/t81/weights.hpp` | `tests/cpp/test_weights.cpp` | 80% | Tier B |
| **REQ-VM-07** | Garbage Collection Determinism | `spec/t81vm-spec.md` | `src/vm/vm.cpp` | `tests/cpp/test_gc_determinism.cpp` | 100% | Tier A |

## 7.3 Property-Based Testing

To verify the mathematical soundness of the custom ternary types, T81 uses property-based testing.

### 7.3.1 Integer Invariants
The test binary `t81_property_invariants_test` checks:
*   **Commutativity**: $a + b = b + a$
*   **Associativity**: $(a + b) + c = a + (b + c)$
*   **Identity**: $a + 0 = a$
*   **Inverse**: $a + (-a) = 0$
*   **Distributivity**: $a \times (b + c) = (a \times b) + (a \times c)$

### 7.3.2 Float Invariants
The test binary `t81_property_float_test` checks:
*   **Monotonicity**: $a < b \implies f(a) \le f(b)$ (for monotonic functions).
*   **Symmetry**: $\sin(-x) = -\sin(x)$.
*   **Canonicty**: `normalize(f) == f`.

## 7.4 The Determinism Gate

The ultimate test of a T81 implementation is the **Determinism Gate**. This script compiles a suite of reference programs and executes them, comparing the output `.tisc` binaries and execution traces against known-good hashes.

### 7.4.1 Running the Gate
```bash
python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
```

### 7.4.2 Gate Failure Analysis
If the gate fails, it indicates a breach of the Sovereign Compute contract.
*   **Binary Mismatch**: The compiler is emitting non-deterministic ASTs or code. Check `src/frontend/compiler.cpp`.
*   **Trace Mismatch**: The VM is executing differently. Check for host-dependent behavior (e.g., `std::unordered_map` iteration order) in `src/vm/vm.cpp`.

## 7.5 Verification Checklists

### 7.5.1 Pre-Release Checklist
*   [ ] All unit tests pass (`ctest`).
*   [ ] `t81_property_invariants_test` passes (1M iterations).
*   [ ] `t81lang_repro_gate` passes on Linux x86_64.
*   [ ] `t81lang_repro_gate` passes on Linux ARM64.
*   [ ] `t81lang_repro_gate` passes on macOS ARM64.

### 7.5.2 Auditor Checklist
*   [ ] Verify that no `Forbidden Operations` (see Chapter 2) are present in the VM source.
*   [ ] Verify that all Axion `VerdictKind::Deny` paths result in an immediate `Trap`.
*   [ ] Verify that `dmath` is used for all transcendental functions in the production build.
