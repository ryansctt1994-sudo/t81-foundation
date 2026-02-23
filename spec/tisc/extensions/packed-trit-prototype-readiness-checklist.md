# Packed-Trit Prototype Readiness Checklist

This checklist tracks the steps required to transition from the "Investigation" phase to a concrete library prototype for Tritwise Operations.

## Phase 1: Library Prototype (C++ / T81) - COMPLETED

- [x] **Define `PackedTritVector` Class**
    - [x] Create a C++ class wrapping `std::vector<uint8_t>` (using PT-5 encoding).
    - [x] Implement `from_trits` / `to_trits` with automatic unpacking/packing.
    - [x] Add `trit_count` property.

- [x] **Implement Core Logical Operations** (Unpack-Op-Repack Strategy)
    - [x] `t_and(a, b)`: Lane-wise Minimum.
    - [x] `t_or(a, b)`: Lane-wise Maximum.
    - [x] `t_xor(a, b)`: Lane-wise Difference (Canonical `lhs - rhs` wrapped).
    - [x] `t_not(a)`: Lane-wise Inversion.

- [ ] **Implement Utility Operations** (Deferred to future phases)
    - [ ] `pop_count(a)`: Count non-zero trits.
    - [ ] `first_non_zero(a)`: Index of first non-zero trit.
    - [ ] `slice(start, len)`: Sub-vector extraction.

## Phase 2A: Compute Representation Prototype - COMPLETED

- [x] **Prototyping Alternative Backings**
    - [x] Implement `ComputeTritVector` (2 bits/trit, 4 trits/byte).
    - [x] Implement `t_and/or/xor/not` on `ComputeTritVector` (unpack-op-repack for semantic parity).
    - [x] Verify semantic equivalence between Scalar, Phase 1 (PT-5), and Phase 2A (2-bit).

- [x] **Verification & Conformance**
    - [x] Create randomized conformance vectors (deterministic seed 42).
    - [x] Verify exact scalar reference matching.
    - [x] **Critical:** Verify `TXor` truth table (9 cases) for non-commutative behavior.
    - [x] Verify cross-representation consistency (`Phase1 == Phase2A`).

- [x] **Benchmark Baseline**
    - [x] Measure throughput of scalar loop implementation.
    - [x] Measure throughput of PT-5 unpack-op-repack loop.
    - [x] Measure throughput of 2-bit unpack-op-repack loop.
    - [x] *Result:* Both packed implementations are significantly slower than scalar due to codec overhead. 2-bit packing requires direct bitwise operations to realize performance benefits.

## Phase 2B: Direct Bitwise Operations (Next Step)

- [ ] **Implement Direct Logic on 2-bit Packing**
    - [ ] Derive boolean formulas for `TAnd`, `TOr`, `TNot`, `TXor` on 2-bit representation.
    - [ ] Implement operations directly on `std::vector<uint8_t>` (SWAR), bypassing unpack/repack.
    - [ ] Benchmark against Scalar and Phase 1.
    - [ ] *Goal:* Demonstrate speedup > 1x vs Scalar (e.g., 4x theoretical peak per byte).

## Phase 3: SIMD Optimization (Future)

- [ ] **SIMD Acceleration**
    - [ ] Implement AVX2/NEON kernels for 2-bit packed operations.
    - [ ] Benchmark large vector throughput.

## Phase 4: Extension Integration (Optional)

- [ ] **Design Extension Detection API**
    - [ ] Define how a T81 program queries for "Tritwise Extension v1".
    - [ ] Draft a fallback strategy (if extension missing -> use library slow path).
