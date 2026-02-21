# Chapter 2: Background and Principles

The design of the T81 ecosystem is not arbitrary; it follows a strict set of philosophical and technical principles laid out in its foundational documents.

## 2.1 The Constitution of the T81 Civilization

As defined in `spec/constitution.md`, the project is guided by four immutable principles:

1.  **Truth shall be provable**
    *   Computation is not just about getting an answer; it is about being able to *prove* that the answer is correct and was derived correctly.
    *   This drives the requirement for the **Axion Trace System**, which provides a cryptographic log of execution.

2.  **Computation shall cost entropy**
    *   Resources (CPU cycles, memory, stack depth) are finite.
    *   The system must explicitly track and account for resource usage. This is why Axion enforces policies like `max-instructions` and `max-recursion`.

3.  **Memory shall be persistent**
    *   Data should not be ephemeral.
    *   This principle underpins **CanonFS**, the content-addressed, immutable filesystem that stores data as canonical blocks identified by their hash.

4.  **Discovery shall be automatic**
    *   The system supports higher-order reasoning.
    *   This informs the **Cognitive Tiers** model, which allows the system to recognize and categorize the complexity of the code it is executing.

## 2.2 The Strict Determinism Profile

Determinism is the "Tier A" requirement for T81. The specification `spec/determinism-profile.md` defines exactly what this entails.

### 2.2.1 Determinism Tiers

| Tier | Name | Description | Host Dependency | Reproducibility |
|---|---|---|---|---|
| **A** | **Strict / Bit-Exact** | Pure integer/ternary logic, software-defined float math. | **None** | **Absolute** |
| **B** | **Canonical Numeric** | Allowed host-optimized float arithmetic (IEEE-754). | Limited (FPU) | High |
| **C** | **Host-Tolerant** | Default mode. Allows host `double`. | Yes | Functional |
| **F** | **Unconstrained** | Debug/Legacy. Access to wall-clock, host RNG. | **High** | **None** |

**Note**: All "Strict Mode" operations in T81 imply **Tier A**.

### 2.2.2 VM-Safe Types (Tier A)

To maintain Tier A compliance, the VM restricts available types:

*   **Safe**: `Trit`, `T81Int`, `T81BigInt`, `T81Fraction`, `T81Option`, `List`, `String`.
*   **Conditional**: `T81Float` is safe *only* if the software backend (`dmath`) is used for transcendental functions (`sin`, `exp`, etc.). Hardware FPU usage is forbidden.
*   **Forbidden**: Raw pointers, Host Time, System Handles (file descriptors).

### 2.2.3 Forbidden Operations

The following are strictly prohibited in Tier A code:
1.  **Host Entropy**: Reading `/dev/random` or unseeded memory.
2.  **Wall-Clock**: `std::chrono::now()` or `gettimeofday()`.
3.  **Address Observability**: Using memory addresses as values (breaks ASLR determinism).
4.  **Unsorted Iteration**: Iterating over hash maps in pointer order.
5.  **Non-Canonical Serialization**: Emitting non-normalized data.

## 2.3 Ternary-Native Design

T81 relies on **Balanced Ternary**, a numeral system using base 3 with digits $\{-1, 0, +1\}$.

### 2.3.1 Why Ternary?

1.  **Symmetry**: The values are centered around zero. Negation is simply inverting the sign of digits ($+ \to -$, $- \to +$, $0 \to 0$).
2.  **Rounding**: Truncation in balanced ternary (dropping lower trits) is mathematically equivalent to rounding to the nearest integer. This eliminates the bias found in binary truncation (floor) and simplifies arithmetic hardware logic.
3.  **Efficiency**:
    *   **Radix Economy**: Base $e$ ($2.718...$) is theoretically the most efficient base for storage. Base 3 is the closest integer to $e$, offering better storage density than Base 2.
    *   **Signed Arithmetic**: There is no need for a separate sign bit or Two's Complement complexity. The sign is inherent in the most significant non-zero trit.

### 2.3.2 Base-81 Encoding

To interact efficiently with binary hardware, T81 groups trits into **Trytes**.
*   **1 Tryte** = 4 Trits ($3^4 = 81$ values).
*   Values range from $-40$ to $+40$.
*   Characters are encoded using a customized Base-81 ASCII set, ensuring human-readable and URL-safe representations.

## 2.4 Software-Defined Math (dmath)

A critical component of T81's determinism is `dmath`. Standard C++ math functions (`std::sin`, `std::pow`) rely on the underlying hardware (x86 SSE/AVX, ARM NEON) and the OS's `libm`. These implementations often differ in the last few bits of precision (ULP - Units in Last Place).

For T81, even a 1-bit difference is a failure. Therefore, T81 implements its own math library in software:
*   **Algorithms**: CORDIC, Taylor Series, and Chebychev approximations with fixed iteration counts.
*   **Consistency**: A `T81Float` calculation on a Raspberry Pi (ARM) yields the *exact* same bit pattern as on a high-end Xeon server (x86).
