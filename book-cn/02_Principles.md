# Chapter 2: Core Principles and Invariants

## 2.1 The Determinism Invariant

**Status: Implemented & Tested**

The central axiom of the T81 architecture is **Strict Determinism**. A T81 program is a pure function $f(S, I) \to S'$, where $S$ is the initial state and $I$ is the input. This function must yield bit-identical $S'$ on any compliant hardware platform.

Achieving this requires eliminating all sources of non-determinism common in modern computing:
*   **Hardware Floating Point**: Replaced by software-defined `T81Float` (`dmath`).
*   **Memory Layout**: Logical addresses are decoupled from physical pointers.
*   **Concurrency**: Thread scheduling is replaced by deterministic coroutines and logical ticks.
*   **System Time**: Wall-clock time is replaced by Lamport timestamps (logical ticks).

### 2.1.1 Determinism Surfaces and Attack Vectors

The following table maps the "surfaces" where non-determinism can leak into the system and the specific mitigations T81 employs.

| Layer    | Determinism Risk             | Mitigation                | Evidence                |
| -------- | ---------------------------- | ------------------------- | ----------------------- |
| Compiler | Token ordering               | Canonical AST emission    | `scripts/ci/t81lang_repro_gate.py` |
| VM       | Memory address leakage       | No address observability  | `src/vm/vm.cpp` (Memory Segments) |
| GC       | Non-deterministic collection | Allocation-count triggers | `src/vm/vm.cpp`: `run_gc_cycle_` |
| Float    | Host FPU drift (IEEE-754)    | `dmath` software float    | `include/t81/core/T81Float.hpp` |
| JIT      | Optimization divergence      | Trace-based equivalence   | `src/vm/jit_compiler.cpp` |

> **Verification**: The JIT compiler in `src/vm/jit_compiler.cpp` ensures that optimized traces exit (`GuardDeopt`) upon *any* state divergence from the interpreted baseline.

## 2.2 Ternary Logic (Base-3)

**Status: Implemented & Tested**

T81 is a **balanced ternary** system. The fundamental unit is the **trit**, with values $\{-1, 0, 1\}$ (often denoted as $-, 0, +$).

### 2.2.1 Why Ternary?
1.  **Symmetric Arithmetic**: Rounding is simply truncation towards the nearest integer, as $0.5$ is not a representable fraction in base-3 without infinite expansion. This simplifies the `dmath` library.
2.  **Information Density**: The radix $3$ is closer to $e \approx 2.718$ than $2$ is, offering the theoretical optimum for integer radix economy ($\text{radix} \times \text{width}$).
3.  **Signed Representation**: Negative numbers do not require a sign bit or Two's Complement. The leading trit indicates the sign naturally.

### 2.2.2 Implementation
In the C++ codebase, trits are packed for efficiency but logically distinct.
*   **Storage**: `T81Int` uses 2 bits per trit in packed form (see `include/t81/packing.hpp`).
*   **Arithmetic**: Operations like `Add`, `Mul` are implemented in `src/vm/vm.cpp` using integer math that simulates balanced ternary carry chains.

## 2.3 Auditability and The Axion Trace

**Status: Implemented & Tested**

Every state transition in T81 is auditable. The **Axion Kernel** produces a cryptographic log of execution called the **Trace**.

### 2.3.1 The Trace Structure
A trace is a sequence of `AxionEvent` records, capturing opcodes, verdicts, and associated data.

> **Reference**: See `include/t81/axion/api.hpp` for the event definitions.

This trace serves as a **Proof of Execution**. By replaying the trace against the initial state, an auditor can verify that:
1.  The computation occurred as claimed.
2.  No safety policies were violated.
3.  The final result is correct.

## 2.4 The Nine Principles (Ethics Enforcement)

**Status: Implemented & Tested**

T81 embeds an immutable ethics layer (The Nine Principles $\Theta_1 \dots \Theta_9$) directly into the VM's policy engine. These are not guidelines but **runtime constraints**.

For example:
*   **$\Theta_7$ (Entropy Containment)**: Prevents infinite resource expansion without explicit `InfExpand` permission.
*   **$\Theta_4$ (Interpretability)**: Mandates that opaque "black box" tensors cannot be emitted without accompanying metadata or symbolic graphs.

> **Implementation**: These checks are performed in `src/axion/ethics.cpp`. A violation results in a `VerdictKind::Deny` and immediate `Trap::SecurityFault`.

## 2.5 Verification Checklist

*   [ ] **Float Consistency**: Does `T81Float` produce identical bit-patterns for transcendental functions (`sin`, `exp`) on all platforms? (Run `tests/cpp/test_T81Float.cpp` and `tests/cpp/test_property_float.cpp`)
*   [ ] **GC Determinism**: Does the Garbage Collector run at exact instruction counts (allocations), not wall time? (Check `kGcInterval` in `src/vm/vm.cpp`)
*   [ ] **Trace Integrity**: Is the Axion log immutable during execution? (Verified by `tests/cpp/axion_log_determinism_test.cpp`)

## 2.6 Formal Audit Matrix

| Principle | Spec Section | Implementation | Test Coverage |
| :--- | :--- | :--- | :--- |
| Strict Determinism | `spec/determinism-profile.md` | `src/vm/vm.cpp` | `tests/cpp/test_property_invariants.cpp` |
| Ternary Logic | `spec/t81-data-types.md` | `include/t81/ternary.hpp` | `tests/cpp/ternary_arith_test.cpp` |
| Auditability | `spec/axion-kernel.md` | `include/t81/axion/api.hpp` | `tests/cpp/test_ethics.cpp` |
