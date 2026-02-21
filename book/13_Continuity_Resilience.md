# Chapter 13: Continuity & Archival Resilience

## 13.1 Overview

This chapter outlines the protocols for **Cleanroom Reconstruction**: the process of rebuilding the T81 Foundation system from zero, assuming total loss of the original development environment, binary artifacts, and team knowledge. The goal is to ensure the **Long-Horizon Continuity** of the project over decades or centuries.

## 13.2 The Cleanroom Reconstruction Protocol

**Objective**: Produce a bit-exact replica of the `t81` binary and verify its deterministic properties using only the source code and this manuscript.

### 13.2.1 Minimum Viable Environment (MVE)
To reconstruct T81, the following tools are required:
1.  **C++ Compiler**: Supporting C++20 standard (e.g., ISO/IEC 14882:2020).
2.  **Standard Library**: A conformant implementation of the C++ Standard Library.
3.  **Build System**: A mechanism to invoke the compiler (e.g., `make`, `cmake`, or manual shell script).
4.  **Python 3**: For running verification scripts and the `t81_python` bindings (optional for core VM).

### 13.2.2 Critical Dependency Map
The T81 codebase minimizes external dependencies to reduce "dependency rot".

| Dependency | Purpose | Criticality | Mitigation Strategy |
| :--- | :--- | :--- | :--- |
| **pybind11** | Python bindings | High (for surface) | Vendor the source code into `third_party/`. |
| **Catch2 / GTest** | Unit testing | Medium | Tests can be rewritten; core logic is independent. |
| **nlohmann/json** | Serialization | Medium | Simple JSON parser can be implemented if needed. |
| **libm** | Math functions | **CRITICAL** | **RISK**: `std::sin` etc. vary by platform. Must replace with `dmath`. |

## 13.3 Single Points of Failure (SPOF)

### 13.3.1 Cryptographic Hash Function
*   **SPOF**: The entire CanonFS relies on **SHA3-256**.
*   **Risk**: If SHA3-256 is broken (preimage attack found), content addressability fails.
*   **Resilience**: The `CanonHash81` struct (`include/t81/hash/canonhash.hpp`) wraps the hash. A future version could introduce a "multihash" prefix to migrate to a new algorithm (e.g., BLAKE3) without breaking old references (though old data would need re-hashing).

### 13.3.2 The Floating Point Backend
*   **SPOF**: The current VM implementation relies on host `double` (IEEE 754).
*   **Risk**: Hardware architecture drift (e.g., move to non-IEEE platforms or different rounding modes).
*   **Resilience**: The **Aspirational Semantics** (Chapter 11) define a software-only ternary float. Implementing this fully in `T81Float.hpp` removes the hardware dependency.

### 13.3.3 The Build Toolchain
*   **SPOF**: Complex build scripts (`CMakeLists.txt`) may become incompatible with future build tools.
*   **Resilience**: Maintain a simple, dependency-free shell script (`build_minimal.sh`) that compiles the core VM (`src/vm/*.cpp`) into a standalone executable.

## 13.4 The Continuity Manifest

The following files constitute the **Immutable Core** of T81. Their logic defines the system's identity.

1.  `include/t81/core/T81Int.hpp`: The definition of the Ternary Integer.
2.  `include/t81/core/T81Float.hpp`: The definition of the Ternary Float.
3.  `src/vm/vm.cpp`: The reference interpreter loop (`Interpreter::step`).
4.  `src/axion/policy_engine.cpp`: The safety kernel logic.
5.  `book/*.md`: The formal specification (this document).

**Preservation Rule**: These files should be printable on archival paper and OCR-able back into source code with high fidelity.

## 13.5 Immutable Formal Invariants

These invariants must **never** change in any future version of T81. If they change, the resulting system is a fork, not a continuation.

1.  **Base-3**: All arithmetic is natively balanced ternary ($-1, 0, 1$).
2.  **Determinism**: $S_{n+1} = \delta(S_n, I)$ is a pure function.
3.  **CanonFS**: All code and data are content-addressed by hash.
4.  **Axion Primacy**: No instruction executes without Axion policy approval.
5.  **Finite Limits**: All recursion and memory allocation must be bounded.

## 13.6 Digital Dark Age Recovery

In the event of a "Digital Dark Age" where complex toolchains (compilers, OSes) are lost:
1.  **Bootstrap**: Implement a minimal C++ subset compiler or a direct TISC interpreter in assembly for the available hardware.
2.  **Transcribe**: Manually transcribe `src/vm/vm.cpp` into the new environment.
3.  **Verify**: Run the "Genesis Block" (a minimal TISC program computing a known integer sequence) to verify correctness.
