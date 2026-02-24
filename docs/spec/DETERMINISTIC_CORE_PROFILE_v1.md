# Deterministic Core Profile v1

**Version:** 1.0
**Status:** Frozen / Active
**Reference:** `docs/governance/FREEZE_ENFORCEMENT.md`

## 1. Definition

The **Deterministic Core Profile (DCP)** defines the minimal subset of the T81 system that is guaranteed to be bit-exact reproducible across all supported architectures (x86-64, ARM64) and operating systems (Linux, macOS).

Components within this profile are subject to strict **Freeze Enforcement**. Components outside this profile are considered "Extended" or "Experimental" and do not carry the same determinism guarantees.

## 2. Core Components

The following table defines the boundaries of the DCP.

| Component | Included | Frozen | Verified | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **TISC ISA** | Yes | Yes | Yes | Opcode semantics, fault behavior. |
| **Data Types** | Yes | Yes | Yes | `Trit`, `Tryte`, `T81Float` (soft-float), `T81BigInt`. |
| **Interpreter** | Yes | Yes | Yes | Standard dispatch loop (`src/vm`). |
| **Soft-Float** | Yes | Yes | Yes | `dmath` library (no native FPU). |
| **Canonical Serialization** | Yes | Yes | Yes | Binary format for types and tensors. |
| **Trace-JIT** | No | No | No | Experimental. See `JIT_EQUIVALENCE_PLAN.md`. |
| **Cognitive Tiers** | No | No | No | Higher-level AI reasoning (Tiers 1-5). |
| **Distributed Compute** | No | No | No | Networked execution. |
| **External Tooling** | No | No | No | `t3k` quantization (Partial), CLI tools. |
| **Experimental Extensions** | No | No | No | Anything in `src/experimental`. |

## 3. Guarantees

For any component marked **Included**:

1.  **Bit-Exactness:** Output must match bit-for-bit on Reference (Zen4) and Apple Silicon (M2).
2.  **Long-Term Stability:** Semantics will not change within Major Version 1.x.
3.  **Audit Trail:** Verification is tracked in `docs/status/VERIFIED_SURFACE_AUDIT.md`.

## 4. Usage

*   **Compiler Targets:** Compilers targeting T81 should default to the DCP instruction set.
*   **Verification:** Auditors should focus primarily on the DCP surface.
*   **Porting:** New platform ports must verify the DCP first using `repro-ledger.yml`.
