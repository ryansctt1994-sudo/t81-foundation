# PackedTritVector Prototype Phase 2C Report

## 1. Summary

Phase 2C of the `PackedTritVector` prototype implements SWAR (SIMD Within A Register) optimizations for the 2-bit packed trit arithmetic operations `TNot`, `TAnd`, and `TOr`. This phase aims to improve the compute throughput of packed trit vectors while maintaining exact canonical ternary semantics. The optimization utilizes 64-bit word-level parallelism to process 32 trits per cycle, significantly reducing the overhead observed in the Phase 2B LUT-based implementation. `TXor` remains on the Phase 2B LUT path due to its non-commutative nature and higher semantic risk.

## 2. Phase 2B Follow-Through Adjustments

*   **Scope:** Phase 2C is strictly a compute-path optimization for the 2-bit `ComputeTritVector` representation.
*   **Roles:** PT-5 (`PackedTritVector`) remains the canonical storage and interchange representation. `ComputeTritVector` serves as the high-performance compute prototype.
*   **Reference Paths:** The Phase 2B LUT implementation has been preserved as a fallback and reference implementation, accessible internally (e.g., `t_and_lut`) for differential testing.

## 3. SWAR-First Packed Optimization Design

### Op-Class Split
The optimization strategy distinguishes between "safe" commutative/bitwise-friendly operations and "risky" non-commutative operations:
*   **Class A (SWAR-Optimized):** `TNot`, `TAnd`, `TOr`. These operations map cleanly to bitwise logic on the 2-bit representation (`00`=0, `01`=1, `11`=-1).
*   **Class B (LUT Fallback):** `TXor` (Ternary Difference). This operation involves a wrap-around difference (`a - b`) which is non-commutative and complex to map efficiently to bitwise logic without excessive branching or instruction count.

### Chunk Strategy
*   **Primary Loop:** Processes data in `uint64_t` chunks (8 bytes, 32 trits) using `std::memcpy` to ensure alignment safety.
*   **Remainder Handling:** A byte-wise fallback loop processes any remaining bytes.
*   **Trailing Bits:** The final byte is masked to ensure unused bits (padding) remain zero, preserving canonical state.

### Invalid State Policy
Phase 2C relies on input validation during construction (`from_trits` rejects invalid trits). The SWAR logic assumes valid 2-bit patterns (`00`, `01`, `11`). Invalid patterns (`10`) are not explicitly handled in the hot path, but the system guarantees they are not introduced by valid operations.

## 4. Implementation of Phase 2C Fast Paths

*   **`TNot`:** Implemented as `x ^ ((x & 0x55...) << 1)`. This correctly toggles the high bit based on the low bit to map `00`→`00`, `01`→`11`, `11`→`01`.
*   **`TAnd` (Min):** Uses a constructed high/low bit formula:
    *   `H = (a | b) & 0xAA...`
    *   `L = (a & b) & 0x55...`
    *   `Res = H | (H >> 1) | L`
    *   This logic correctly implements `-1 < 0 < 1`.
*   **`TOr` (Max):** Uses a similar constructed formula:
    *   `H = h_a & h_b`
    *   `L = (l_a & l_b) | ((l_a | l_b) & ~(h_a | h_b))`
    *   `Res = H | (H >> 1) | L`

**Wrapper Selection:** The public methods `t_not`, `t_and`, `t_or` automatically use the SWAR implementation.

## 5. `TXor` Path Decision and Verification

**Decision:** **LUT Fallback.**
The `TXor` operation remains implemented via the Phase 2B LUT path (`t_xor_lut`).

**Justification:**
*   `TXor` is semantically `wrap(a - b)`, which is non-commutative.
*   Complexity of bitwise mapping for ternary difference is high.
*   Risk of semantic divergence outweighs the potential speedup for this specific operation in this phase.
*   The LUT path is proven correct and "fast enough" (2.1 µs vs 90 µs for naive) for current needs compared to the risk.

## 6. Conformance and Safety Verification

*   **Differential Tests:** `test_phase2c_swar_equivalence` verifies that `t_not`, `t_and`, `t_or` (SWAR) produce identical results to the Reference implementation across multiple sizes (including non-aligned sizes like 1027).
*   **Explicit LUT Tests:** `test_phase2b_lut_explicit` verifies the preserved LUT paths (`t_not_lut`, etc.) still function correctly against Reference.
*   **Edge Cases:** Trailing byte masking is verified to ensure padding remains clean.
*   **Randomized Testing:** Large randomized vectors confirm robustness.

## 7. Benchmark Comparisons

Benchmarks were run on `ComputeTritVector` for Size=4096 trits.

| Operation | Phase 2A (Naive) | Phase 2B (LUT) | Phase 2C (SWAR) | Scalar Reference | Speedup (2C vs 2B) | Speedup (2C vs 2A) |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **TAnd** | 90.75 µs | 2.12 µs | **0.159 µs** | 0.150 µs | **13.3x** | **571x** |
| **TNot** | ~32 µs | 2.17 µs | **0.105 µs** | 0.110 µs | **20.6x** | ~300x |
| **TOr** | ~92 µs | 2.11 µs | **0.145 µs** | 0.149 µs | **14.5x** | ~630x |
| **TXor** | ~105 µs | 2.11 µs | 2.11 µs (LUT) | 1.89 µs | 1.0x (Same) | ~50x |

*Note: Scalar Reference values are for the same logical operation count. Phase 2A/2B/2C include vector allocation overhead.*

**Interpretation:**
*   **Massive Speedup:** Phase 2C SWAR provides a massive 13-20x speedup over the already optimized Phase 2B LUT implementation.
*   **Scalar Parity:** Phase 2C achieves parity with (and sometimes beats) the Scalar Reference implementation. This is significant because Packed vectors use 4x less memory bandwidth. The Scalar reference is likely auto-vectorized by the compiler, so matching it with explicit packed SWAR confirms the efficiency of the 2-bit representation.
*   **TXor:** Remains the bottleneck among packed operations, but is still 50x faster than the naive implementation.

## 8. Validation Results

*   [x] Phase 2C docs preserve PT-5 (storage) vs 2-bit (compute prototype) distinction
*   [x] Phase 2B LUT path preserved as reference/fallback
*   [x] SWAR-first fast path implemented for `TNot`, `TAnd`, `TOr`
*   [x] `TXor` path decision is explicit (LUT fallback)
*   [x] Differential tests compare Phase 2C vs Reference
*   [x] `TXor` exactness guarded by LUT fallback
*   [x] Edge lengths/final-byte masking behavior re-verified
*   [x] Benchmarks compare scalar / Phase 2A / Phase 2B / Phase 2C
*   [x] Performance claims are conservative (parity with scalar, huge win over LUT)
*   [x] Phase 2C report created

## 9. Phase 2D Recommendations and Remaining Gaps

*   **Native SIMD:** With SWAR achieving scalar parity, further optimization via AVX2/NEON intrinsics (Phase 2D) is the logical next step to beat scalar performance decisively.
*   **Allocator Reuse:** The current benchmarks include vector allocation overhead (~100-150ns). Implementing an in-place mutation API or allocator reuse would expose the raw compute throughput even more.
*   **TXor Optimization:** Future work could explore a dedicated SIMD/SWAR path for `TXor` if it becomes a hotspot, potentially using shuffle instructions or a derived bitwise formula.
