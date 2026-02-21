# Chapter 12: Adversarial & Failure Modeling

## 12.1 Overview

This chapter extends the T81 Threat Model to specifically address **Adversarial Determinism**: attacks designed to force the VM to produce different outputs on different hosts, thereby breaking consensus or auditability.

## 12.2 Determinism Attack Vectors

### 12.2.1 The Compiler Attack Surface
*   **Attack**: Compiler generates non-deterministic bytecode.
*   **Vector**: Iterating over `std::unordered_map` or `std::set` (pointer-based order) when emitting symbol tables or code.
*   **Manifestation**: `t81lang` produces different `.tisc` binaries on Linux vs macOS for the same source.
*   **Mitigation (Implemented)**: `src/frontend/compiler.cpp` must sort all maps by key before emission.
*   **Status**: Checked by `t81lang_repro_gate.py`.

### 12.2.2 The VM Host Interface (The "Libm Gap")
*   **Attack**: Exploiting platform-specific floating-point behavior.
*   **Vector**: The `FSin`, `FCos`, `FExp` opcodes currently invoke `std::sin`, `std::cos` etc. from the host's `libm`.
*   **Vulnerability**: GLIBC, MUSL, and MSVC implementations of transcendental functions differ in the last bit (ULP).
*   **Exploit**: An attacker constructs a program that branches on `sin(x) == expected_value`.
*   **Result**: The program returns `True` on Host A and `False` on Host B. Consensus failure.
*   **Mitigation (Aspirational)**: Replace `std::sin` with `t81::core::detail::dmath::sin` (software implementation).
*   **Current State**: **VULNERABLE** (Tier C determinism).

### 12.2.3 The Garbage Collector Timing Attack
*   **Attack**: Forcing GC to run at different times.
*   **Vector**: The `run_gc_cycle_` function is triggered by `instructions_since_gc_ >= kGcInterval` (64).
*   **Vulnerability**: If `kGcInterval` logic depends on *wall-clock time* or *host memory pressure*, execution diverges.
*   **Mitigation (Implemented)**: GC is strictly deterministic based on instruction count (`instructions_since_gc_`).
*   **Status**: **SECURE**.

### 12.2.4 The CanonFS Preimage Attack
*   **Attack**: Loading malicious data disguised as a valid object.
*   **Vector**: `Opcode::TLoadHash` loads data by SHA3-256 hash.
*   **Vulnerability**: Hash collision (theoretically computationally infeasible for SHA3-256).
*   **Real Risk**: Implementation bug in `CanonHash81::from_string` or storage layer allowing content swapping.
*   **Mitigation**: CanonFS verifies the hash of content *after* loading from disk.

### 12.2.5 The Distributed Time-Travel Attack
*   **Attack**: Desynchronizing the logical clock of a node.
*   **Vector**: `Opcode::TickSync` calls `NodeState::sync_tick(remote_tick)`.
*   **Code Reference**: `src/cog/tier4/distributed.cpp`:
    ```cpp
    void NodeState::sync_tick(uint64_t remote_tick) {
      if (remote_tick > vector.global_tick) {
        vector.global_tick = remote_tick;
      }
    }
    ```
*   **Exploit**: A malicious peer sends a message with `tick = UINT64_MAX`.
*   **Result**: The victim node's clock jumps to max value, potentially causing integer overflows in subsequent logic or preventing valid updates (if logic relies on `current < max`).
*   **Mitigation (Required)**: Axion Policy must cap the maximum allowable tick jump (e.g., `+1000` per sync).
*   **Status**: **VULNERABLE** (Experimental Tier 4).

## 12.3 Determinism Breach Postmortem Template

Use this template when a `repro_gate` failure or consensus break occurs.

### Incident Report: [INCIDENT-ID]

**1. Detection**
*   **Date**: YYYY-MM-DD
*   **Trigger**: CI Failure / User Report / Audit
*   **Affected Components**: [Compiler / VM / JIT / StdLib]

**2. Divergence Manifest**
*   **Host A (Reference)**: Linux x86_64, GCC 11
*   **Host B (Divergent)**: macOS ARM64, Clang 14
*   **Differing Output**:
    ```
    Host A: 0.1234567890123456
    Host B: 0.1234567890123457
    ```

**3. Root Cause Analysis**
*   **Hypothesis**: [e.g., `FAdd` associativity difference]
*   **Code Trace**:
    - File: `src/vm/vm.cpp`
    - Line: [Line Number]
    - Opcode: [Opcode Name]
*   **Verification**: [Describe minimal reproduction case]

**4. Mitigation Plan**
*   **Immediate Fix**: [e.g., disable `-ffast-math`]
*   **Long-term Fix**: [e.g., implement software float add]
*   **Test Case**: [Link to new regression test]

**5. Impact Assessment**
*   **Severity**: [Critical / Major / Minor]
*   **Consensus Risk**: [Yes / No]

## 12.4 Recommendations for Hardening

1.  **Eliminate `double`**: Prioritize the full integration of `T81Float` in the VM registers and heap.
2.  **Sanitize Inputs**: Implement strict validation for `TickSync` and `Gossip` payloads.
3.  **Fuzzing**: Add a fuzzer that specifically targets the JIT vs Interpreter equivalence (`src/vm/jit_compiler.cpp`).
