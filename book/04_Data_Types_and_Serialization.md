# Chapter 4: Data Types and Serialization

## 4.1 Primitive Types

**Status: Implemented & Tested**

T81 defines a strict set of primitive data types. These types are designed to be efficiently mappable to host hardware while preserving the semantics of balanced ternary logic.

| Type | Size | Description |
| :--- | :--- | :--- |
| `T81Int` | 64-bit | A signed integer representing a ternary value. Can be viewed as a 27-trit word. |
| `T81Float` | 64-bit | A deterministic floating-point number with custom rounding and transcendental behavior. |
| `T81Bool` | 8-bit | A boolean value: True (1), False (0), or Unknown (-1). |
| `Char` | 32-bit | A Unicode codepoint (UTF-32). |
| `Handle` | 64-bit | An opaque reference to a heap-allocated object (Tensor, Graph). |

### 4.1.1 T81Int (Packed Trits)
The `T81Int` is the workhorse of integer arithmetic. Internally, it is stored as a standard `int64_t` for performance, but the TISC ISA treats it as a sequence of trits when performing bitwise operations.

*   **Logic**: Balanced Ternary.
*   **Range**: $\approx \pm 3.8 \times 10^{12}$ (matches 27 trits).
*   **Representation**: Two's complement binary simulation.

### 4.1.2 T81Float (Deterministic Float)
The `T81Float` type is critical for the "Libm Gap" solution.
*   **Storage**: IEEE-754 `double` (64-bit) layout is used for *storage* to allow zero-copy loading in some cases.
*   **Semantics**: Operations (`+`, `-`, `*`) map to `dmath::add`, `dmath::sub`, etc., which enforce:
    *   **Rounding**: Round-to-nearest, ties-to-even.
    *   **NaN Handling**: Canonical NaN payload (always `0x7ff8000000000000`).
    *   **Subnormals**: Flushed to zero (FTZ) or preserved, strictly defined by the build configuration (default: preserved).

## 4.2 T81Float and dmath

**Status: Implemented & Tested**

The `dmath` library provides the implementation for `T81Float`. It bypasses the host FPU for all transcendental functions to ensure bit-exact results.

### 4.2.1 Transcendental Implementation
Functions like `sin(x)` are implemented using Taylor Series expansions with fixed coefficients.

$$
\sin(x) \approx x - \frac{x^3}{3!} + \frac{x^5}{5!} - \frac{x^7}{7!} + \dots
$$

*   **Range Reduction**: Arguments are reduced to $[-\pi, \pi]$ using a high-precision value of $\pi$ stored as `uint64_t` arrays.
*   **Polynomial Eval**: Horner's method is used for evaluation.
*   **Constants**: All constants ($1/3!$, $\pi$) are hardcoded in hex to prevent compiler-dependent parsing of decimal literals.

## 4.3 Tensors and Canonical Layouts

**Status: Implemented**

T81 is designed for AI workloads. The `T81Tensor` is a first-class citizen.

### 4.3.1 Tensor Memory Layout
To ensure that tensor operations (like matrix multiplication) are deterministic, the memory layout must be rigid.
*   **Row-Major**: All tensors are stored in row-major order (C-style), not column-major (Fortran-style).
*   **Contiguity**: Tensors are contiguous blocks of memory. No strided views are supported in the core `T81Tensor` type (views are handled by higher-level abstractions).
*   **Alignment**: The data pointer is always aligned to 64 bytes to support AVX-512 (if the host supports it, purely for speed, not affecting the result).

### 4.3.2 SafeTensors Integration
T81 adopts the `safetensors` format for on-disk storage. This format is zero-copy friendly and prevents pickle-based code execution attacks. CanonFS validates the SHA3-256 hash of the `.safetensors` file before mapping it.

## 4.4 Canonical Serialization Rules

**Status: Implemented**

When data leaves the VM (e.g., for the Audit Trace or Network Gossip), it must be serialized canonically.

1.  **Integers**: Little-endian 64-bit.
2.  **Floats**: IEEE-754 64-bit binary representation.
3.  **Strings**: UTF-8, no BOM, null-terminated.
4.  **Maps/Dictionaries**: **Sorted by Key**.
    *   Before hashing a JSON-like object, all keys are sorted lexicographically.
    *   `{"b": 1, "a": 2}` $\to$ `{"a": 2, "b": 1}`.
5.  **Lists/Vectors**: Preserved order.

This ensures that $Hash(\text{Serialize}(A)) == Hash(\text{Serialize}(B))$ if and only if $A$ and $B$ are semantically identical.
