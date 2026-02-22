# Chapter 11: Formal Semantics

## 11.1 Operational Semantics

**Status: Specification**

The operational semantics of T81 are defined as a state transition system.

$$ S' = \text{VM}(S, \text{Op}) $$

Where $S$ is the machine state $(R, PC, SP, M_{seg}, \Phi)$.

### 11.1.1 The Transition Function
The transition function is deterministic. For any state $S$ and opcode $\text{Op}$, there is exactly one valid next state $S'$ or a fault condition $\bot$.

This property is verified by the **Determinism Gate** (`scripts/ci/t81lang_repro_gate.py`), which ensures that the interpreter yields identical transitions across platforms.

## 11.2 Memory Semantics

**Status: Implemented**

Memory in T81 is not a linear array of bytes but a structured store of **Typed Objects**.
*   **Safety**: Accessing a tensor element out of bounds is not undefined behavior; it is a guaranteed `TypeFault` or `StackFault`.
*   **Immutability**: Once a tensor is committed to CanonFS, it is immutable.

> **Verification**: `tests/cpp/vm_bounds_test.cpp` ensures that all out-of-bounds accesses trap correctly.
