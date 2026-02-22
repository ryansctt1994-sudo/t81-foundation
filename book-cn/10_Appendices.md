# Chapter 10: Appendices

## 10.1 What Is Not Yet Implemented

While the core T81 architecture is stable, several features remain in experimental or aspirational states as of February 2026.

1.  **Fully Deterministic Transcendentals (Phase 2)**:
    *   Currently, inverse trigonometric functions (`asin`, `acos`, `atan`) and hyperbolic functions (`sinh`, `cosh`, `tanh`) rely on the host's `libm` unless `T81_DETERMINISTIC` is set (which disables them or returns errors).
    *   **Goal**: Implement `dmath` support for all transcendental functions.

2.  **Advanced CanonFS Features**:
    *   Currently, CanonFS supports basic content-addressable loading.
    *   **Missing**: Distributed pinning, peer-to-peer replication, and garbage collection of unreferenced artifacts.

3.  **Trace-JIT Maturity**:
    *   The Trace-JIT (`src/vm/jit_compiler.cpp`) is functional but considered **Experimental**. It does not yet cover all opcodes and may fallback to the interpreter frequently.

4.  **Full Tier 1 Symbolic Algebra**:
    *   Basic symbolic graph support exists (`src/cog/tier1/symbolic.cpp`), but full algebraic rewriting and simplification (CAS capabilities) are not yet exposed via standard opcodes.

5.  **Holotensor Types**:
    *   Mentioned in early specs as a high-dimensional sparse tensor format. Currently, only dense `T729Tensor` and `T81Tensor` are implemented.

## 10.2 Error Codes

| Code | Name | Description |
| :--- | :--- | :--- |
| `0x00` | `Ok` | Success. |
| `0x01` | `SecurityFault` | Axion policy violation. |
| `0x02` | `TypeFault` | Invalid operand type. |
| `0x03` | `StackFault` | Stack overflow/underflow. |
| `0x04` | `MathFault` | Division by zero or domain error. |

## 10.3 Useful Links

*   **Repository**: [github.com/t81dev/t81-foundation](https://github.com/t81dev/t81-foundation)
*   **Specification**: `spec/` directory in the repo.
*   **Issues**: GitHub Issues tracker.
