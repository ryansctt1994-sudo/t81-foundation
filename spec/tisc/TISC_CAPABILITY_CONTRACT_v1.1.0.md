# TISC Capability Contract (v1.1.0)

**Version:** 1.1.0 (Canonical Freeze)
**Date:** 2025-02-15
**Authority:** T81 Foundation / TISC Architecture Board

## 1. Purpose
This document defines the strict capability boundary of the TISC v1.1.0 ISA. It serves as the "Source of Truth" for AI agents, compiler writers, and VM implementers.

## 2. Frozen Guarantees

Any compliant TISC v1.1.0 implementation **MUST** guarantee:

1.  **Opcode Stability**: The numeric encoding of opcodes `0x00` through `0xAD` (0–173) is immutable.
2.  **Instruction Width**: All instructions are exactly 13 bytes (1 byte opcode + 3x 4-byte little-endian operands).
3.  **Bitwise Semantics**:
    *   `BitShl`, `BitShr`, `BitUShr` **MUST** mask the shift amount with `0x3F`.
    *   `BitShr` **MUST** perform arithmetic shift (sign extension).
    *   `BitUShr` **MUST** perform logical shift (zero fill).
4.  **Reserved Range**: Opcodes `0xAE` (174) through `0xFF` (255) are reserved. Execution of these opcodes **MUST** trap or be rejected by the loader.

## 3. Conditional Guarantees (Determinism)

*   **Integer Operations**: Bit-exact deterministic on all 64-bit platforms.
*   **Floating Point**:
    *   Basic ops (`FAdd`, `FSub`, `FMul`, `FDiv`) are deterministic assuming IEEE 754 compliance.
    *   **Transcendental Functions** (`FSin`, `FExp`, etc.) are **implementation-defined** by default (host `libm`).
    *   **Strict Determinism** is only guaranteed if the VM is compiled with `T81_DETERMINISTIC`, forcing software emulation of math functions.

## 4. Known Stubs & Non-Final Behavior

The following opcodes preserve the ISA encoding but offer limited or stubbed functionality in v1.1.0:

| Category | Opcodes | Behavior |
| :--- | :--- | :--- |
| **Network** | `NSend`, `NRecv`, `VWait`, `VYield` | No-op / Log Axion Event. No actual I/O. |
| **Neural** | `TNeuralFwd` | Identity (Pass-through). |
| **Neural** | `TNeuralBwd` | No-op / Log Axion Event. |
| **Axion** | `AxSign`, `AxLineage`, `AxCanon` | No-op / Log Axion Event. |

Tools SHOULD NOT rely on these opcodes for production logic in v1.1.0.

## 5. Out of Scope / Deferred

The following features are **NOT** present in TISC v1.1.0:

*   **Bitwise Rotate**: `RotL`, `RotR` (Polyfill with shifts).
*   **Population Count**: `PopCount`.
*   **Hashing**: Single-instruction SHA-3.
*   **Filesystem**: No direct FS access (except via `TLoadHash` for immutable tensors).

## 6. Usage Contract

*   **Compiler Writers**: You MAY target opcodes 0–173. You MUST NOT emit opcodes 174–255. You MUST polyfill rotates.
*   **VM Implementers**: You MUST implement the full dispatch table for 0–173. You MUST treat 174–255 as invalid/trap.
*   **Future Extensions**: New opcodes will only be added in the reserved range (174+) or in a new major version (v2.0).
