# RFC Tritwise Extension Gate Evidence

## Executive Summary

**Status:** Outcome B (Continue Investigation)

This document summarizes the evidence collected from the `tritwise` library adoption and workload benchmarks. The goal is to determine if a TISC ISA extension (new opcodes) is warranted or if the library acceleration is sufficient.

## Workload Evidence

We implemented a representative workload suite (`BM_TritwiseWorkloads`) covering 4 scenarios:
1.  **Ternary Mask Apply**: Large vector `AND` (3x speedup).
2.  **Packed Pattern Match**: Chained `AND/OR` (1.6x speedup).
3.  **Canonicalization Pipeline**: Repeated `NOT/OR/AND`.
4.  **Tensor-Lane Emulation**: Many small vector operations.

### Performance Data

| Workload | Scalar Baseline | Library Accelerated | Speedup | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **Mask Apply** | 2.72 µs | 0.85 µs | **3.2x** | Strong SIMD gain on large vectors. |
| **Pattern Match** | 4.88 µs | 2.94 µs | **1.66x** | Moderate gain (chained ops). |
| **Canon Pipeline** | 2.84 µs | 3.12 µs | 0.91x | Regression due to profiling overhead + mixed sizes. |
| **Tensor Lane** | 8.15 µs | 39.24 µs | 0.20x | **Severe regression** on small vectors (<64B). |

*Note: The severe regression in `Tensor Lane` is attributed to the `T81_TRITWISE_PROFILE` instrumentation lock contention (1000 calls per iteration).*

### Profiling Statistics

Profiling data collected during benchmark runs:

*   **Total Ops Tracked**: ~315 Million calls.
*   **Total Data Processed**: ~153 GB.
*   **Size Distribution**:
    *   **< 64 Bytes**: 118 Million calls (37%) - *Critical Path*
    *   **64 - 256 Bytes**: 88 Million calls (28%)
    *   **> 256 Bytes**: 109 Million calls (35%)

## Analysis

1.  **Large Vectors (>= 256 trits):** The library implementation provides significant acceleration (3x), validating the Phase 2E SIMD backend. The library is **sufficient** for these workloads.
2.  **Small Vectors (< 64 bytes):** These account for 37% of calls. The current library approach (with function call and dispatch overhead) coupled with profiling shows high costs.
3.  **Profiling Overhead:** The mutex-based profiling heavily skewed the small-vector results. To make a final decision, we need clean numbers without instrumentation.

## Decision: Outcome B (Continue Investigation)

**Rationale:**
While the library proves highly effective for bulk processing (Mask Apply), the high frequency of small-vector operations (37%) combined with the observed regression in `Tensor Lane` suggests that function call and dispatch overheads are non-negligible.

We cannot rule out the need for an ISA extension yet. An ISA extension (TISC opcodes) might allow the VM to dispatch these small operations more efficiently than a native library call boundary, or at least justify specific "Micro-Ops" for small fixed-size trits.

**Next Steps:**
1.  Re-run benchmarks *without* profiling instrumentation to measure true small-vector overhead.
2.  Profile within a TISC VM context (using `CallNative` vs hypothetical Opcode dispatch simulation).
3.  **Do not freeze ISA yet**, but **do not draft opcodes** until small-vector performance is resolved.

The `tritwise` library should be adopted as the standard native implementation, but the door remains open for ISA elevation.
