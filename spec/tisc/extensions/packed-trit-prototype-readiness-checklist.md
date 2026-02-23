# Packed-Trit Prototype Readiness Checklist

This checklist tracks the steps required to transition from the "Investigation" phase to a concrete library prototype for Tritwise Operations.

## Phase 1: Library Prototype (C++ / T81)

- [ ] **Define `PackedTritVector` Class**
    - [ ] Create a C++ class wrapping `std::vector<uint8_t>` (using PT-5 encoding).
    - [ ] Implement `get_trit(index)` and `set_trit(index, value)` with automatic unpacking/packing.
    - [ ] Add `trit_count` property.

- [ ] **Implement Core Logical Operations**
    - [ ] `TritAnd(a, b)`: Lane-wise Minimum.
    - [ ] `TritOr(a, b)`: Lane-wise Maximum.
    - [ ] `TritXor(a, b)`: Lane-wise Sum/Mod (Canonical).
    - [ ] `TritNot(a)`: Lane-wise Inversion.

- [ ] **Implement Utility Operations**
    - [ ] `PopCount(a)`: Count non-zero trits.
    - [ ] `FirstNonZero(a)`: Index of first non-zero trit (for masking).
    - [ ] `Slice(start, len)`: Sub-vector extraction.

## Phase 2: Verification & Conformance

- [ ] **Generate Conformance Vectors**
    - [ ] Create a script that generates random trit sequences.
    - [ ] Compute expected results using scalar logic (reference implementation).
    - [ ] Verify that `PackedTritVector` operations match scalar results exactly.
    - [ ] **Edge Case:** Mismatched lengths (pad with 0 vs trap?).
    - [ ] **Edge Case:** Zero-length vectors.

- [ ] **Benchmark Baseline**
    - [ ] Measure throughput (Trits/sec) of scalar loop implementation.
    - [ ] Measure throughput of PT-5 unpack-op-repack loop.
    - [ ] *Goal:* Establish the "cost of abstraction".

## Phase 3: Optimization Investigation

- [ ] **Prototyping Alternative Backings**
    - [ ] Implement `PackedTritVector2Bit` (2 bits/trit, no division).
    - [ ] Benchmark `TritAnd` on 2-bit vs PT-5.
    - [ ] *Hypothesis:* 2-bit packing will be 5-10x faster for logical ops but 50% less dense.

## Phase 4: Extension Integration (Optional)

- [ ] **Design Extension Detection API**
    - [ ] Define how a T81 program queries for "Tritwise Extension v1".
    - [ ] Draft a fallback strategy (if extension missing -> use library slow path).
