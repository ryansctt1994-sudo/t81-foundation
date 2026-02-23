# Release Notes: TISC v1.1.0 (Canonical Freeze)

**Date:** 2025-02-15
**Status:** Frozen / Long-Term Stable
**Canonical Opcode Count:** 174

## Overview

The TISC v1.1.0 release marks the **Canonical Freeze** of the Ternary Instruction Set Computer (TISC) ISA. This version establishes a rigid boundary for the core instruction set, guaranteeing opcode stability for compilers, virtual machines, and hardware implementations.

This freeze locks in **174 canonical opcodes** (0x00–0xAD), ensuring that the numeric encoding of these instructions will never change.

## Key Changes

### 1. Bitwise Integer Operations (RFC-Implementation)
Seven new bitwise instructions have been added to the Core ISA to support integer manipulation and cryptography primitives.

*   **`BitAnd` (0xA7)**: Bitwise AND
*   **`BitOr` (0xA8)**: Bitwise OR
*   **`BitXor` (0xA9)**: Bitwise XOR
*   **`BitNot` (0xAA)**: Bitwise NOT
*   **`BitShl` (0xAB)**: Bitwise Shift Left
*   **`BitShr` (0xAC)**: Bitwise Shift Right (Arithmetic / Sign-Extending)
*   **`BitUShr` (0xAD)**: Bitwise Shift Right (Logical / Zero-Filling)

**Determinism Guarantee:** Shift amounts are strictly masked with `& 0x3F` (63) before execution. This ensures consistent behavior regardless of host architecture or undefined C++ behavior for out-of-range shifts.

### 2. Neural & Axion Primitives
The following opcodes are frozen as **Functional Stubs** or **Identity Operations** to reserve their encoding space while allowing the runtime to evolve:

*   **Neural**: `TNeuralFwd` (Identity/Pass-through), `TNeuralBwd` (Log-only).
*   **Axion**: `AxSign`, `AxLineage`, `AxCanon` (Log-only events).
*   **Network**: `NSend`, `NRecv`, `VWait`, `VYield` (Simulation stubs).

### 3. Reserved Range
The opcode range **174 (0xAE) to 255 (0xFF)** is explicitly reserved for future standardization. Implementations MUST NOT assign custom semantics to these values.

## Determinism Caveats

While integer and control-flow operations are bit-exact deterministic, **floating-point transcendental functions** (e.g., `FSin`, `FExp`) currently rely on the host `libm` for performance.

*   **Strict Determinism**: Requires building with `-DT81_DETERMINISTIC=ON`, which engages software-defined math libraries (slower but bit-exact).
*   **Default Behavior**: Uses hardware/host instructions.

## Deferred Extensions

The following proposed extensions are **NOT** included in the v1.1.0 Core ISA and remain deferred:

*   Bitwise Rotate (`RotL`, `RotR`)
*   Population Count (`PopCount`)
*   Hardware SHA-3 (`Sha3`)

## Migration Guide

Developers targeting TISC v1.0 should update their assemblers and VMs to support the new bitwise opcodes. See `docs/migration/TISC_v1.1.0_Bitwise_Ops_Migration.md` for implementation details.
