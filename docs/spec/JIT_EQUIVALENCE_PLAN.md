# JIT Equivalence Plan

**Status:** Experimental / Planned
**Category:** Determinism Extension
**Scope:** T81VM Trace-JIT

## 1. Objective

To introduce Just-In-Time (JIT) compilation to the T81VM while preserving strict bit-exact determinism. This document defines the formal equivalence contract that the JIT compiler must satisfy to be considered a verified surface.

## 2. Equivalence Definition

The JIT compiler is correct if and only if, for any initial state $S_0$ and input $I$, the state transition sequence produced by the JIT execution is bit-identical to that produced by the reference interpreter.

$$ \forall S_0, I : \text{Interpreter}(S_0, I) \xrightarrow{n} S_n \iff \text{JIT}(S_0, I) \xrightarrow{n} S_n $$

Where $S_n$ includes:
*   Register values ($R_0 \dots R_{80}$)
*   Memory content (Stack, Heap, Tensor, Meta)
*   Axion trace events
*   Fault conditions

## 3. Canonical Trace Hashing Model

To verify equivalence without comparing entire state dumps at every step, the JIT must emit a **Canonical Trace Hash** that aggregates the execution history.

### Mechanism
1.  **Checkpointing:** At every basic block boundary or Axion event, the JIT updates a running hash of the architectural state.
    $$ H_{t+1} = \text{Hash}(H_t || \text{Opcode} || \text{Operands} || \text{Result}) $$
2.  **Verification:** This hash $H_{final}$ is compared against the reference interpreter's hash for the same execution path.

### Constraints
*   The hashing mechanism itself must be deterministic and platform-independent.
*   Float operations in the JIT must match the soft-float behavior of the interpreter exactly.

## 4. Required Test Expansion

To upgrade the JIT status from "Experimental" to "Verified", the following test coverage is required:

1.  **Opcode Exhaustion:** Every TISC opcode compiled by the JIT must be fuzz-tested against the interpreter with random inputs.
2.  **Edge Cases:**
    *   Stack overflow/underflow boundaries.
    *   Self-modifying code attempts (must fault identically).
    *   Axion instruction interception.
    *   Floating-point singularities (NaN, Inf handling, if applicable).
3.  **Cross-Platform Parity:** The JIT must produce identical trace hashes on x86-64 and ARM64.

## 5. Known Risks

*   **Host FPU Divergence:** Using native FPU instructions instead of soft-float libraries will break determinism. The JIT must either emit calls to the soft-float library or use SIMD instructions that guarantee bit-exact compliance with `T81Float`.
*   **Optimization Drift:** Reordering instructions (e.g., dead code elimination, loop unrolling) might alter the sequence of Axion trace events or fault timing.
    *   *Mitigation:* The JIT is forbidden from optimizing across "Observable Boundaries" (Axion calls, memory IO).

## 6. Preconditions for "Verified" Status

The JIT surface will remain **Experimental** until:
1.  The `JIT(S) == Interpreter(S)` property is formally verified for all implemented opcodes.
2.  The `repro-ledger.yml` CI workflow includes JIT-enabled runs in the determinism matrix.
3.  A "JIT Compliance Audit" is successfully completed and signed off by maintainers.
