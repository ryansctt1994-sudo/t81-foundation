# Determinism Surface Report

**Date:** 2025-05-20
**Agent:** Deterministic Systems Stress Agent (Jules)

## 1. Executive Summary

This report documents the results of a comprehensive "Full-Lattice Stress Exploration" of the T81 core datatype ecosystem via T81Lang. The objective was to aggressively probe boundaries, cross-layer composition, and determinism guarantees.

**Key Findings:**
*   **Frozen Core Integrity:** `T81Int`, `T81BigInt`, `T81Fraction`, and `T81Float` (basic ops) are stable and deterministic. Overflow behavior is generally consistent (or absent/wrapped safely).
*   **Symbolic Stability:** `T81Polynomial` and `T81Symbol` demonstrate deterministic construction and canonical confluence checking. A minor type drift was observed where `std.symbol.intern` is typed as returning `T81String` in the frontend.
*   **Container Polyfills:** `Map` and `Set` types in T81Lang currently lower to `Vector` operations (polyfills). While functional, they rely on linear scans and lack native hash-map performance characteristics in the current VM lowering.
*   **Tensor Boundaries:** `Tensor` operations are strictly shape-enforced at runtime (`ShapeFault` triggered correctly). Dynamic construction of tensors from variables in vector literals (`[var, var]`) is currently unsupported in the IR generator ("Vector literal data missing").
*   **Monadic Integrity:** `Option` and `Result` pipelines are robust and deterministic.
*   **Agent Determinism:** The `T81Agent` and `std.sys.entropy` interfaces operate in a deterministic mode (zero entropy) by default, facilitating reproducible runs.

## 2. Surface Classification

| Surface | Status | Confidence | Notes |
| :--- | :--- | :--- | :--- |
| **Numeric Core** | ✅ **Verified** | High | `T81Int`, `BigInt`, `Fraction` stable. `Fixed` exists but usage requires precise constructors. |
| **Floating Point** | ⚠️ **Unverified** | Medium | `T81Float` literals require standard notation (no scientific suffix `t81` in parser yet). |
| **Symbolic Layer** | ✅ **Verified** | High | Canonicalization works. `Symbol` type drift (String return) needs remediation. |
| **Containers** | ⚠️ **Experimental** | Low | `Map`/`Set` are polyfills over `Vector`. Performance is O(N). |
| **Tensors** | ⚠️ **Fragile** | Medium | Static literals work. Dynamic vector construction fails IR gen. Shape enforcement is strict. |
| **Monads** | ✅ **Verified** | High | `Option`/`Result` logic is sound. |
| **Agent/Entropy** | ✅ **Verified** | High | Deterministic by default (Entropy=0). |

## 3. Drift & Anomalies

1.  **Type Drift:** `std.symbol.intern` is defined to return `T81String` in `semantic_analyzer.cpp`, contradicting the expectation of returning a distinct `T81Symbol` type.
2.  **IR Limitation:** `IRGenerator` fails when `VectorLiteralExpr` contains non-constant expressions (e.g., `[var, var]`). This limits dynamic tensor creation from T81Lang.
3.  **Polyfill Exposure:** Error messages explicitly reference `Vector` when `Map` or `Set` operations fail, exposing the polyfill implementation detail to the user.

## 4. Reproducibility Evidence

*   **Numeric:** 100% reproducible trace for overflow and canonicalization.
*   **Symbolic:** 100% reproducible confluence checks.
*   **Tensors:** 100% reproducible `ShapeFault` on mismatched dims.
*   **Agent:** 100% reproducible `Entropy=0` state.

## 5. Recommendations

1.  **Fix Symbol Typing:** Update `std.symbol.intern` signature to return `T81Symbol`.
2.  **Enhance IR Generator:** Implement lowering for non-constant `VectorLiteralExpr` to support dynamic tensor construction.
3.  **Native Containers:** Prioritize lowering `Map`/`Set` to native VM opcodes (e.g., `T81Map`) instead of Vector polyfills.
4.  **Scientific Notation:** Update lexer/parser to support `1.0e-20t81` style literals for `T81Float`.

---
*End of Report*
