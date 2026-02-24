# Experimental Surface Inventory

**Status:** Active Inventory
**Last Updated:** 2026-02-24
**Reference:** `docs/spec/DETERMINISTIC_CORE_PROFILE_v1.md`

This document lists all components, features, and subsystems that are **NOT** part of the frozen, verified core.
These areas are subject to change without notice and carry no determinism guarantees.

## Experimental Inventory

| Component | Location | Freeze Scope | Determinism Scope | Stability |
| :--- | :--- | :--- | :--- | :--- |
| **Cognitive Tiers** | `src/cog/`, `src/tiers/` | None (Evolving) | None (Planned) | **Experimental** |
| **Hanoi VM Kernel** | `src/hanoi/` | None (Evolving) | None (Planned) | **Experimental** |
| **JIT Compiler** | `src/vm/jit_compiler.cpp` | None (Experimental) | Verified per Plan | **Alpha / Stub** |
| **Distributed Compute** | `src/cog/tier4/` | None (Network) | None (Nondeterministic) | **Experimental** |
| **Experimental Headers** | `include/t81/experimental/` | None | None | **Experimental** |
| **Notebooks** | `notebooks/` | None | None | **Example Only** |
| **Examples** | `examples/` | None | None | **Example Only** |

## Isolation Status

### 1. JIT Compiler
*   **Status**: Present in `src/vm/jit_compiler.cpp` but disabled by default.
*   **Isolation**: Must be explicitly enabled via build flag or runtime config.
*   **Risk**: High determinism risk if enabled.

### 2. Cognitive Tiers (Axion/Hanoi)
*   **Status**: Partially implemented in `src/cog/` and `src/hanoi/`.
*   **Isolation**: Separate namespace `t81::cog`, `t81::hanoi`.
*   **Risk**: Low impact on core VM unless invoked.

### 3. Distributed Compute
*   **Status**: Headers exist (`tier4/distributed.hpp`), implementation stubbed.
*   **Isolation**: Dependent on network libraries not linked in core.
*   **Risk**: None (compile-time isolation).

## Governance Rule

Per `docs/governance/FREEZE_ENFORCEMENT.md`:
> Experimental components MUST NOT be linked from normative specs or core architecture docs as a dependency.
> They MUST NOT affect the behavior of the `DETERMINISTIC_CORE_PROFILE` unless explicitly opted-in.
