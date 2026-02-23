# Packed-Trit Representation Audit and Comparison

## 1. Summary

This document provides an audit of the existing packed-trit and ternary encoding implementations within the T81 repository. It serves as a foundational analysis for future "tritwise" library or runtime extensions by establishing the ground truth of current representations.

**Key Findings:**
*   The repository contains three primary packed representations: **PT-5** (dense byte-stream), **Base-81 Digits** (semi-dense, text-aligned), and **Base-81 Packed32** (intermediate 32-bit blocks).
*   All implementations enforce a consistent canonicalization strategy: **Zero-padding** (neutral trit) for partial groups, little-endian trit ordering within groups, and explicit `trit_count` metadata for length recovery.
*   **PT-5** is the superior candidate for storage and transport due to 94.9% efficiency.
*   **Base-81** is the canonical text/interchange format.

## 2. Codec Discovery and File Mapping

The following files constitute the authoritative codec implementation:

| File Path | Role | Key Types/Functions |
| :--- | :--- | :--- |
| `include/t81/codec/trit_packing.hpp` | **Core API** for PT-5 and Base-81 stream packing. | `pack_pt5`, `unpack_pt5`, `pack_base81`, `unpack_base81` |
| `src/codec/trit_packing.cpp` | **Implementation** of core packing logic. | Padding logic, validation, loop unrolling. |
| `include/t81/codec/base81.hpp` | **Text Encoding** (Bytes $\leftrightarrow$ String). | `encode_bytes`, `decode_bytes` (Base-81 alphabet). |
| `include/t81/codec/base81_packed.hpp` | **Intermediate Packing** (High-density blocks). | `pack5`, `unpack5` (20 trits $\to$ `uint32_t`). |
| `include/t81/codec/metadata.hpp` | **Container Format** (Binary with Header). | `Header`, `wrap_encoded_buffer` (Magic, Version, `trit_count`). |
| `include/t81/codec/base243.hpp` | **BigInt/Legacy** (Base-243 digits). | `encode_bytes_be` (Big-endian byte $\to$ digit conversion). |

## 3. Extracted Representation Behaviors

Analysis of `src/codec/trit_packing.cpp` and related headers reveals the following hard-coded behaviors:

### A. Trit Domain and Bias
*   **Input Domain:** Trits are `{-1, 0, +1}` (signed).
*   **Internal Rebiasing:** All packing functions rebias trits to `{0, 1, 2}` before packing.
    *   Formula: $val = t_{raw} + 1$
*   **Validation:** Input trits outside `{-1, 0, +1}` trigger `PackingError::InvalidTritValue`.

### B. Grouping and Density

#### 1. PT-5 (Packed Trytes-5)
*   **Grouping:** 5 trits per byte (8 bits).
*   **Capacity:** $3^5 = 243$ states (values $0..242$).
*   **Efficiency:** $\log_2(243) / 8 \approx 94.9\%$.
*   **Formula:** $Byte = \sum_{i=0}^4 (t_i+1) \cdot 3^i$.

#### 2. Base-81 Digits
*   **Grouping:** 4 trits per digit (byte).
*   **Capacity:** $3^4 = 81$ states (values $0..80$).
*   **Efficiency:** $\log_2(81) / 8 \approx 31.6\%$ (storage), but $100\%$ aligned to Base-81 text alphabet.
*   **Formula:** $Digit = \sum_{i=0}^3 (t_i+1) \cdot 3^i$.

#### 3. Base-81 Packed32
*   **Grouping:** 5 Base-81 digits (20 trits) per `uint32_t`.
*   **Capacity:** $81^5 = 3^{20} \approx 3.48 \times 10^9$.
*   **Efficiency:** $\log_2(3^{20}) / 32 \approx 99.0\%$ of the utilized `uint32` range, but effectively same density as PT-5 if stored as bytes.
*   **Role:** Intermediate processing / alignment.

### C. Ordering and Endianness
*   **Intra-Group Ordering:** **Little-Endian**.
    *   The first trit ($t_0$) in the stream is the Least Significant Trit (LST) of the packed byte/digit.
    *   $Byte = (t_0+1) + 3(t_1+1) + \dots$
*   **Stream Ordering:** Bytes are emitted in stream order.

### D. Padding and Partial Groups
*   **Padding Strategy:** **Implicit Zero-Padding**.
    *   When `trit_count` is not a multiple of the group size (5 or 4), the implementation treats missing trits as `0` (Neutral).
    *   Since `0` is rebiased to `1`, the padding contributes to the value.
    *   *Example:* Packing 1 trit `[-1]` into PT-5.
        *   $t_0=-1 \to 0$.
        *   $t_1..t_4$ (missing) $\to 0$ (Default `t_raw=0`).
        *   Byte value: $0 + 3(1) + 9(1) + 27(1) + 81(1) = 0 + 3 + 9 + 27 + 81 = 120$.
        *   Wait, code verification:
            *   `int t_raw = 0; // Default`
            *   `int val = t_raw + 1; // = 1`
            *   Yes, padding is effectively adding `1`s in higher powers of 3.

### E. Metadata and Canonicalization
*   **Length Authority:** The `trit_count` in `t81::codec::metadata::Header` is authoritative.
*   **Round-Trip Identity:**
    *   `Unpack(Pack(trits))` $\to$ Identity (exact match).
    *   `Pack(Unpack(bytes))` $\to$ **Canonicalizes** the byte stream. If the original bytes had non-standard padding (e.g., representing 'unknown' instead of '0'), the re-packing will overwrite it with standard zero-padding.
*   **Metadata Header:**
    *   Magic: `0x43313854` ('T81C')
    *   Version: `1`
    *   Includes `EncodingType` and `trit_count`.

## 4. Packed Representation Comparison and Role Analysis

| Feature | PT-5 (Canonical) | Base-81 Digits | Base-81 Packed32 | Scalar / Unpacked |
| :--- | :--- | :--- | :--- | :--- |
| **Density** | **High (~95%)** | Low (~32%) | High (~99% of u32) | Very Low (1 trit/byte or word) |
| **Alignment** | Byte-aligned | Byte-aligned | 32-bit aligned | Word aligned |
| **Encoding Cost** | Moderate (Div/Mul) | Low (Div/Mul) | Moderate | None |
| **Text Ready** | No (Binary) | **Yes (1:1 Map)** | No | No |
| **Simplicity** | Moderate | Simple | Complex | **Trivial** |
| **Existing Use** | **Storage/Transport** | **I/O & Display** | Internal | **Runtime Compute** |

### Role Recommendations

1.  **Storage & Transport:** **PT-5**.
    *   *Reason:* Highest byte-stream density without bit-level complexity. Canonical in repo.
2.  **Text / Interchange:** **Base-81**.
    *   *Reason:* 1-to-1 mapping with ASCII alphabet. Used for user-facing strings.
3.  **Runtime Compute (Future):** **Unpacked or SIMD-Optimized (2-bit)**.
    *   *Observation:* None of the existing packed formats are ideal for lane-wise arithmetic (e.g., `TAnd` on packed trits) because they require expensive unpacking (division) to access individual trits.
    *   *Recommendation:* Future "tritwise" ops should likely use a **2-bit unpacked** representation (2 bits per trit, 32 trits per `uint64`) or keep using scalar registers for simplicity, converting to PT-5 only for storage.

## 5. Working Canonicalization and Determinism Proposal (Docs-Only)

To ensure consistent behavior for future prototyping, the following contract is proposed (based on existing code):

### A. Canonical Trit Domain
*   Logical trits are integers `{-1, 0, +1}`.
*   `0` is the Neutral/Unknown element.

### B. Representation Invariance
*   Operations must behave identically regardless of the underlying packing.
*   `Unpack(Pack(T))` must equal `T`.

### C. Padding Semantics
*   **Rule:** When packing a sequence of length $N$, any padding required to fill the final storage unit must be semantically equivalent to appending **Neutral Trits (`0`)**.
*   **Validation:** Decoders must ignore padding values based on the authoritative `trit_count`.
*   **Forbidden:** Padding must not be used to store metadata or sentinel values.

### D. Byte-Level Determinism
*   Two packed streams are equal if and only if their `trit_count`s are equal and their byte sequences are identical.
*   Because padding is deterministic (Zero-fill), there is only one valid byte sequence for a given trit sequence under a fixed encoding (PT-5 or Base-81).

## 6. Prototype Readiness Recommendations

For the investigation of Tritwise Operations ([RFC-tritwise-ops-extension-investigation.md](RFC-tritwise-ops-extension-investigation.md)), we recommend the following staged approach:

1.  **Phase 1: Library Prototype (No ISA Changes)**
    *   Define a C++ class `PackedTritVector` backed by `std::vector<uint8_t>` (PT-5).
    *   Implement `And`, `Or`, `Xor`, `Not` by unpacking, operating, and repacking.
    *   *Goal:* Establish semantic correctness and test vectors.

2.  **Phase 2: Optimized Storage Format**
    *   Investigate a "Runtime Packed" format (e.g., **2-bit encoding**: `00`=0, `01`=1, `11`=-1) that allows bitwise emulation of ternary logic without division.
    *   Compare performance against the PT-5 unpack-process-repack loop.

3.  **Phase 3: Extension RFC**
    *   Only propose new opcodes if Phase 2 demonstrates >5% speedup on real workloads (e.g., ternary neural net inference) compared to optimized library code.
