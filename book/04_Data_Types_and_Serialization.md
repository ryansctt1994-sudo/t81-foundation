# Chapter 4: Data Types and Serialization

## 4.1 Primitive Types

**Status: Implemented & Tested**

The T81 architecture is built upon a foundation of balanced ternary primitives. These types are designed to be efficiently simulated on binary hardware while maintaining the mathematical properties of base-3 logic.

### 4.1.1 Trits and Trytes

*   **Trit**: The fundamental atom of information, taking values $\{-1, 0, 1\}$.
    *   Also denoted as $\{T, 0, 1\}$ or $\{-, 0, +\}$.
    *   Information content: $\log_2 3 \approx 1.58$ bits.
*   **Tryte**: A sequence of trits. The standard "Byte equivalent" is the 4-trit Tryte ($3^4 = 81$ values).
*   **Word**: A standard T81 machine word is 27 trits ($3^{27} \approx 7.6 \times 10^{12}$), which fits comfortably within a 64-bit integer.

**Packed Representation (T3_K)**:
For efficient storage, trits are often packed using a 2-bit code:
*   `00` $\to$ 0
*   `01` $\to$ 1
*   `10` $\to$ -1 (T)
*   `11` $\to$ Unused / Padding

### 4.1.2 T81Int (Arbitrary Precision Integer)
`T81Int` is a variable-width integer type. Unlike binary integers which use Two's Complement for negative values, balanced ternary integers are symmetric.

*   **Range**: Symmetric interval $[-\frac{3^N-1}{2}, +\frac{3^N-1}{2}]$.
*   **Normalization**: Leading zeros are strictly forbidden in the canonical serialized form. The only valid representation for Zero is a single `0` trit.

## 4.2 T81Float and dmath

**Status: Implemented (Core)**

Floating-point arithmetic is the primary source of non-determinism in cross-platform computing. T81 replaces hardware IEEE-754 floats with a fully software-defined format: **`T81Float`**.

### 4.2.1 Canonical Definition
A `T81Float<M, E>` is a tuple $(s, m, e)$ representing the value:
$$
V = s \times m \times 3^{e - \text{bias}}
$$
where:
*   $s \in \{-1, 1\}$ is the sign (stored as a trit).
*   $m$ is the mantissa, an $M$-trit integer normalized such that the most significant trit is non-zero (unless $V=0$).
*   $e$ is the exponent, an $E$-trit integer.
*   $\text{bias} = \frac{3^E - 1}{2}$.

**Special Values**:
*   **Zero**: $e = 0, m = 0$.
*   **Infinity**: $e = e_{\max}, m = 0$.
*   **NaE (Not an Entity)**: $e = e_{\max}, m \neq 0$. (Equivalent to NaN).

### 4.2.2 The dmath Backend
To guarantee bit-exact results across x86, ARM, and RISC-V, T81 implements **`dmath`** (Deterministic Math).
*   **Arithmetic**: `Add`, `Sub`, `Mul`, `Div` are implemented using integer math on the mantissas, with precise rounding rules (ties-to-even) applied in software.
*   **Transcendentals**: Functions like `sin`, `cos`, `exp`, `log` are computed using **Taylor Series expansions** with a fixed number of iterations and fixed constant precision. This eliminates reliance on the host OS's `libm`, which varies between glibc, musl, and MSVC.

## 4.3 Tensors and Canonical Layouts

**Status: Implemented & Tested**

Tensors (`T81Tensor`) are the workhorses of the cognitive tiers. To support efficient execution and canonical hashing, they follow a strict layout.

### 4.3.1 Memory Layout
Tensors are stored in **Row-Major** order (C-style), not Column-Major (Fortran-style).
*   **Shape**: A vector of dimensions $(d_0, d_1, \dots, d_n)$.
*   **Stride**: Calculated as $s_i = \prod_{j=i+1}^n d_j$.
*   **Alignment**: Tensor data is aligned to 64-byte boundaries in the `Tensor` memory segment to facilitate SIMD loading (AVX-512 / NEON) where safe.

### 4.3.2 Serialization (.t81w)
The `.t81w` (T81 Weights) format is the standard container for persisting tensor models. It is designed to be **mmap-friendly** and **canonical**.

**Binary Structure (Version 2)**:
1.  **Magic Header**: `0x54383157` ("T81W").
2.  **Version**: `0x02`.
3.  **Table of Contents**: A list of `(Hash, Offset, Length)` tuples, sorted by Hash.
4.  **Blob Data**: Contiguous tensor data, padding to 64-byte alignment.

### 4.3.3 Quantization (T3_K)
T81 supports a native ternary quantization format called **T3_K**.
*   **Block Size**: $K$ trits (typically 64 or 128).
*   **Representation**: Each value is quantized to $\{-1, 0, 1\}$.
*   **Scaling**: Each block has a scaling factor (T81Float) to approximate the original magnitude.

## 4.4 Canonical Serialization Rules

**Status: Implemented**

To ensure consistent hashing (for `CanonRef`), all data must be normalized before serialization. The serializer enforces a **Bijective Mapping** between abstract values and byte sequences.

1.  **Integers (T81Int)**:
    *   Strip leading zeros.
    *   Zero is encoded as a single byte `0x00` (assuming specific encoding).
2.  **Floats (T81Float)**:
    *   Must be normalized (max shift left).
    *   Negative Zero is strictly forbidden; it must be converted to Positive Zero.
    *   NaE payloads are zeroed out (no "signaling" vs "quiet" bit differentiation).
3.  **Collections**:
    *   **Maps/Dictionaries**: Keys must be sorted lexicographically by their canonical binary representation.
    *   **Sets**: Elements must be sorted.
4.  **Graphs**:
    *   Nodes are re-indexed by a canonical topological sort. If the graph has cycles, a deterministic tie-breaking rule (based on edge weights) is applied.

> **Verification**: `tests/cpp/test_property_invariants.cpp` verifies these properties via property-based testing (fuzzing), ensuring that $Serialize(Deserialize(Serialize(X))) \equiv Serialize(X)$.
