# Chapter 2: Core Principles and Invariants

## 2.1 The Determinism Invariant

**Status: Implemented & Tested**

The central axiom of the T81 architecture is **Strict Determinism**. In this system, a program $P$ is not a suggestion to the hardware; it is a mathematical definition of a state transition function $f$.

Formally, given an initial state $S$ and an input vector $I$, the function must satisfy:
$$
\forall \text{hardware } H: \text{Exec}_H(S, I) \to S' \implies S' \text{ is invariant}
$$

Achieving this requires eliminating all sources of non-determinism common in modern computing. T81 treats the host environment (OS, CPU, FPU) as an "adversarial entropy source" that must be constrained.

### 2.1.1 Determinism Surfaces and Attack Vectors

The "Determinism Surface" is the boundary where the abstract machine interacts with physical reality. Any leakage of physical reality (time, random noise, hardware quirks) into the logical state constitutes a **Determinism Breach**.

| Layer    | Determinism Risk             | Mitigation Mechanism      | Implementation Evidence |
| :--- | :--- | :--- | :--- |
| **Compiler** | Token ordering, map iteration | Canonical AST sorting | `src/frontend/ast.cpp` (Sorted Maps) |
| **VM Memory** | Pointer address leakage | Opaque Handles (Indices) | `src/vm/vm.cpp` (Memory Segments) |
| **Garbage Collector** | Non-deterministic collection cycles | Alloc-count triggering | `src/vm/gc.cpp` (Instruction-based GC) |
| **Concurrency** | Race conditions, scheduling | Cooperative coroutines | `src/vm/scheduler.cpp` (Deterministic Ticks) |
| **Floating Point** | Host FPU drift (IEEE-754) | `dmath` software float | `include/t81/core/T81Float.hpp` |
| **Transcendental** | Libm implementation variance | Taylor Series (Fixed iter) | `include/t81/core/detail/dmath.hpp` |
| **JIT** | Optimization divergence | Trace Equivalence Checks | `src/vm/jit_compiler.cpp` |

### 2.1.2 The "Libm Gap" and `dmath`
A critical vulnerability in cross-platform determinism is the "Libm Gap". The IEEE-754 standard defines floating-point formats but leaves transcendental functions (sin, cos, pow) loosely specified. As a result, `std::sin(x)` on x86_64/GLIBC may differ by 1 ULP (Unit in the Last Place) from `std::sin(x)` on ARM64/MUSL.

T81 solves this with **`dmath`** (Deterministic Math), a custom library that implements:
*   **Soft-Float Arithmetic**: `Add`, `Sub`, `Mul` are bit-exact.
*   **Custom Transcendentals**: `Sin`, `Cos`, `Exp` are implemented via Taylor/Maclaurin series with a fixed number of iterations and fixed constants, ignoring the host's `libm`.
*   **Rounding Mode**: Ties-to-even is enforced in software.

> **Invariant**: $\text{dmath::sin}(x)$ produces the exact same bit pattern on an Intel i9, an Apple M3, and a RISC-V development board.

## 2.2 Ternary Logic (Base-3)

**Status: Implemented & Tested**

T81 is a **balanced ternary** system. The fundamental unit is the **trit**, with values $\{-1, 0, 1\}$ (often denoted as $-, 0, +$ or $T, 0, 1$).

### 2.2.1 Why Ternary?
1.  **Symmetric Arithmetic**: The value range is symmetric around zero. In binary (Two's Complement), the range is asymmetric (e.g., -128 to +127). In balanced ternary, an $N$-trit integer covers $-\frac{3^N-1}{2} \dots +\frac{3^N-1}{2}$.
2.  **rounding Efficiency**: Rounding to the nearest integer is equivalent to truncation. $0.5$ is not exactly representable, avoiding the "0.5 rounding problem."
3.  **Radix Economy**: The radix economy $E(r, N) = r \lfloor \log_r N \rfloor$ is minimized when $r = e \approx 2.718$. The integer $3$ is closer to $e$ than $2$ is, making ternary theoretically more efficient for information storage density.
4.  **Signed Representation**: Negative numbers do not require a separate sign bit. The sign is carried by the most significant non-zero trit.

### 2.2.2 Implementation
In the C++ codebase, trits are simulated on binary hardware for efficiency.
*   **Packed Storage**: `T81Int` uses a 2-bit-per-trit encoding scheme (00=0, 01=1, 11=-1/T). This allows 4 trits to fit in a byte (a Tryte).
*   **Arithmetic**: Operations are implemented using integer math that simulates balanced ternary carry chains.
    *   Example: $1 + 1 = 1T$ (which is $3 - 1 = 2$).
    *   Example: $T + T = T1$ (which is $-3 + 1 = -2$).

## 2.3 Auditability and The Axion Trace

**Status: Implemented & Tested**

Determinism alone is insufficient; the execution must be **auditable**. The Axion Kernel produces a cryptographic log called the **Trace**.

### 2.3.1 The Trace Structure
A trace $\mathcal{T}$ is an ordered sequence of events $E_0, E_1, \dots, E_k$. Each event captures a significant state transition or policy check.

```cpp
struct AxionEvent {
    uint64_t tick;          // Logical timestamp
    Opcode op;              // The operation attempted
    Verdict verdict;        // The kernel's decision (Allow/Deny)
    CanonHash81 state_hash; // Merkle root of the VM state
    std::string metadata;   // Contextual debug info
};
```

This trace serves as a **Proof of Execution**. By replaying the trace against the initial state, an auditor can mathematically prove that the computation yielded the claimed result without trusting the hardware that produced it.

## 2.4 The Nine Principles (Ethics Enforcement)

**Status: Implemented & Tested**

T81 embeds a set of immutable "Constitutional Principles" ($\Theta_1 \dots \Theta_9$) directly into the VM's policy engine. These are not merely guidelines; they are runtime constraints enforced by the Axion Kernel.

| Symbol | Principle | Description | Enforced By |
| :--- | :--- | :--- | :--- |
| $\Theta_1$ | **Non-Harm** | Fundamental safety layer; prevents memory corruption and segfaults. | Memory Bounds Checks |
| $\Theta_2$ | **Non-Coercion** | Prevents forced state transitions without cryptographic authorization. | Signature Verification |
| $\Theta_3$ | **Truth** | Information must be canonical; no two different hashes can map to the same object. | CanonFS Collision Checks |
| $\Theta_4$ | **Interpretability** | Opaque "black box" execution is warned against; trace generation is mandatory for Tier 3+. | Trace Logger |
| $\Theta_5$ | **Identity Integrity** | Distributed nodes must maintain consistent identity keys. | Tier 4 Handshake |
| $\Theta_6$ | **Ethical Priority** | Safety policies override performance optimizations. | Policy Pre-emption |
| $\Theta_7$ | **Entropy Containment** | Prevents unbounded resource expansion (e.g., infinite loops, memory leaks). | Recursion Limits / Gas |
| $\Theta_8$ | **Canonical Consistency** | All data must be normalized before hashing. | Serializer |
| $\Theta_9$ | **Transparent Execution** | The system must not hide side effects; `MetaWrite` requires explicit policy allowance. | Axion Interceptor |

> **Example**: If a program attempts to recurse infinitely, it violates $\Theta_7$ (Entropy Containment). The Axion Kernel detects that `recursion_depth > policy.max_depth` and issues a `Deny` verdict, converting the operation into a `Trap::SecurityFault`.

## 2.5 Verification Checklist

*   [ ] **Float Consistency**: Does `T81Float` produce identical bit-patterns for transcendental functions (`sin`, `exp`) on all platforms? (Run `tests/cpp/test_T81Float.cpp` and `tests/cpp/test_property_float.cpp`)
*   [ ] **GC Determinism**: Does the Garbage Collector run at exact instruction counts (allocations), not wall time? (Check `kGcInterval` in `src/vm/vm.cpp`)
*   [ ] **Trace Integrity**: Is the Axion log immutable during execution? (Verified by `tests/cpp/axion_log_determinism_test.cpp`)
*   [ ] **Ethics Enforcement**: Do the $\Theta$ checks fire correctly when limits are exceeded? (Verified by `tests/cpp/test_ethics.cpp`)

## 2.6 Formal Audit Matrix

| Principle | Spec Section | Implementation | Test Coverage |
| :--- | :--- | :--- | :--- |
| Strict Determinism | `spec/determinism-profile.md` | `src/vm/vm.cpp` | `tests/cpp/test_property_invariants.cpp` |
| Ternary Logic | `spec/t81-data-types.md` | `include/t81/ternary.hpp` | `tests/cpp/ternary_arith_test.cpp` |
| Auditability | `spec/axion-kernel.md` | `include/t81/axion/api.hpp` | `tests/cpp/test_ethics.cpp` |
| Canonical Storage | `spec/canonfs-spec.md` | `src/canonfs/` | `tests/cpp/canonfs_driver_test.cpp` |
