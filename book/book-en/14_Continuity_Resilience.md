# Chapter 14: Continuity and Resilience

## 14.1 The Cleanroom Protocol

**Status: Aspirational**

The T81 project is designed with a **"Civilization-Scale"** mindset. The goal is that if all source code repositories (GitHub, GitLab, PyPI) were to vanish, the system could be reconstructed from this monograph and a standard C++ compiler specification.

### 14.1.1 Reconstruction Steps
1.  **Retrieve**: Obtain a copy of the **Definitive Technical Monograph** (this book), preserved in IPFS, printed form, or data crystals.
2.  **Verify**: Confirm the cryptographic hashes of the core algorithms (SHA3-256, Balanced Ternary Arithmetic) against known mathematical constants.
3.  **Implement**:
    *   Write a C++23 compliant compiler.
    *   Implement `T81Int` and `T81Float` according to the bit-layout specifications in Chapter 4.
    *   Implement the TISC VM instruction loop (Chapter 3).
    *   Implement the Axion policy logic (Chapter 9).
4.  **Validate**: Run the test suite (`tests/cpp/*.cpp`) included in the appendix or reconstructed from the descriptions.
5.  **Bootstrap**: Use the new implementation to compile the T81Lang self-hosted compiler (if available) or the reference examples.

## 14.2 Single Points of Failure

**Status: Mitigated**

T81 identifies and mitigates reliance on centralized infrastructure.

*   **Source Control**: The repository is mirrored across multiple git forges (GitHub, GitLab) and decentralized networks (IPFS/Radicle).
*   **Build Tools**: CMake is the standard build system, but the project structure is simple enough for manual compilation or shell scripts (`gcc src/*.cpp -o t81`).
*   **Dependencies**: T81 has **zero required runtime dependencies** beyond the C++ standard library. It vendors critical components (like `asio` for networking) or implements them from scratch (like `dmath` for transcendentals). This "hermetic build" philosophy ensures long-term viability.

## 14.3 Continuity Manifest

**Status: Documented**

The following artifacts constitute the "Continuity Kit" necessary to rebuild T81:

1.  **The Book**: `book/book-en/*.md` (This document).
2.  **The Spec**: `spec/*.md` (Formal TISC/Axion specifications).
3.  **The Code**: `src/` and `include/` (Reference implementation).
4.  **The Tests**: `tests/cpp/` (Validation logic).
5.  **The Scripts**: `scripts/ci/` (Reproduction gates).

## 14.4 Immutable Formal Invariants

**Status: Eternal**

Regardless of implementation details (C++, Rust, Zig), any system calling itself "T81" must adhere to these invariants:

1.  **Strict Determinism**: $f(S, I) \to S'$ is bit-exact across platforms.
2.  **Ternary Native**: Logic is base-3.
3.  **Policy Enforced**: No instruction executes without Axion approval.
4.  **Structurally Honest**: No approximations without explicit typing.

If a system violates any of these, it is a fork, not T81. This definition protects the semantic integrity of the project against "embrace, extend, extinguish" tactics.
