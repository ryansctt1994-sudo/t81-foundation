# Chapter 4: Data Types and Serialization

## 4.1 Primitive Types

**Status: Implemented & Tested**

The T81 architecture is built upon a foundation of balanced ternary primitives. These types are designed to be efficiently simulated on binary hardware while maintaining the mathematical properties of base-3 logic.

### 4.1.1 Trits and Trytes
*   **Trit**: The fundamental atom of information, taking values $\{-1, 0, 1\}$.
*   **Tryte**: A sequence of trits. The standard tryte width is 4 trits ($3^4 = 81$ values), often packed into a `uint8_t` for storage.

> **Implementation**: `include/t81/ternary.hpp` defines the `Trit` enum and conversion logic.

### 4.1.2 T81Int (Arbitrary Precision Integer)
`T81Int` is a variable-width integer type using a packed balanced ternary representation.
*   **Storage**: 2 bits per trit.
*   **Range**: Symmetric around zero ($-\frac{3^N-1}{2} \dots +\frac{3^N-1}{2}$).
*   **Normalization**: Leading zeros are strictly forbidden in the canonical serialized form. A zero value is represented by a single zero trit.

> **Verification**: `tests/cpp/test_t81int.cpp` and `tests/cpp/test_property_invariants.cpp`.

## 4.2 T81Float and dmath

**Status: Implemented (Core) / Partial (Extended)**

Floating-point arithmetic is the primary source of non-determinism in cross-platform computing (due to IEEE-754 variances in FMA fusion, transcendental precision, etc.). T81 addresses this via `T81Float`.

### 4.2.1 Canonical Definition
A `T81Float` is a tuple $(m, e)$, representing the value $m \times 3^e$.
*   $m$: Mantissa (T81Int).
*   $e$: Exponent (T81Int).
*   **Invariant**: The mantissa $m$ must be normalized such that its most significant trit is non-zero, unless the value is exactly zero.

### 4.2.2 The dmath Backend
To achieve **Strict Determinism**, the VM employs `dmath` (Deterministic Math), a software-defined arithmetic library.
*   **Core Operations**: `Add`, `Sub`, `Mul` are exact and deterministic (implemented in `T81Float.hpp`).
*   **Transcendentals**: `Sin`, `Cos`, `Tan`, `Exp`, `Log`, `Sqrt` are computed using `dmath` (Taylor series with fixed iteration counts), guaranteeing bit-exact results on any architecture.
*   **Extended Functions**: `Asin`, `Acos`, `Sinh`, `Pow` currently rely on host `double` precision (unless `T81_DETERMINISTIC` is defined, in which case they may return `NaE` or use slow software emulation).

> **Verification**: `tests/cpp/test_T81Float.cpp` validates the correctness of special values and transcendentals. `include/t81/core/detail/dmath.hpp` contains the implementation.

## 4.3 Tensors and Canonical Layouts

**Status: Implemented & Tested**

Tensors (`T729Tensor`, `T81Tensor`) are the workhorses of the cognitive tiers.

### 4.3.1 Memory Layout
Tensors are stored in **Row-Major** order.
*   **Shape**: A vector of dimensions $(d_0, d_1, \dots, d_n)$.
*   **Stride**: Calculated as $s_i = \prod_{j=i+1}^n d_j$.
*   **Alignment**: Tensor data is aligned to 64-byte boundaries in the `Tensor` memory segment.

### 4.3.2 Serialization (.t81w)
The `.t81w` (T81 Weights) format is the standard container for persisting tensor models. Version 2 (`T81W2`) supports quantization and canonical hashing.

**Binary Structure**:
1.  **Magic Header**: `0x54383157` ("T81W").
2.  **Version**: `0x02`.
3.  **Table of Contents**: List of `(Hash, Offset, Length)` tuples.
4.  **Blob Data**: Contiguous tensor data.

**Quantization Formats**:
*   **F32**: Standard IEEE-754 float (canonicalized).
*   **T3_K**: 2-bit-per-trit packing with block-wise scaling.

> **Source**: `include/t81/weights.hpp` and `include/t81/tensor.hpp`.

## 4.4 Canonical Serialization Rules

**Status: Implemented**

To ensure consistent hashing (`CanonRef`), all data must be normalized before serialization.

1.  **BigInt**: Strip leading zeros. Zero is `[0]`.
2.  **Fraction**:
    *   Reduce to lowest terms: $\gcd(num, den) = 1$.
    *   Denominator must be positive.
    *   Zero is $0/1$.
3.  **Float**:
    *   Standardize mantissa/exponent.
    *   NaN payloads are zeroed.
    *   Negative zero is normalized to positive zero.
4.  **Map/Dictionary**:
    *   Keys must be sorted lexicographically by their canonical binary representation.
5.  **Graph**:
    *   Nodes are re-indexed by topological sort order to ensure graph isomorphism yields identical byte streams.

> **Verification**: `tests/cpp/test_property_invariants.cpp` verifies these normalization properties via property-based testing.
