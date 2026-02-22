# Chapter 5: Installation and Build

## 5.1 Prerequisites

To build T81 from source (cleanroom reconstruction), you need:
*   **C++ Compiler**: Clang 18+ or GCC 14+ (C++23 support required).
*   **Build System**: CMake 3.25+.
*   **Python**: Python 3.10+ (for validation scripts).

## 5.2 Build Procedure

1.  **Clone the Repository**:
    ```bash
    git clone https://github.com/t81dev/t81-foundation.git
    cd t81-foundation
    ```

2.  **Configure and Build**:
    ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --parallel
    ```

3.  **Verify the Build**:
    Run the determinism gate script to ensure your toolchain produces bit-exact binaries relative to the reference hashes.
    ```bash
    python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
    ```

## 5.3 Determinism Gate

The `repro_gate.py` script is the primary arbiter of build correctness. It compiles a suite of reference programs (`tests/fixtures/t81lang_determinism`) and compares the resulting TISC bytecode hashes against a known-good `repro.json` manifest.

> **Source**: `scripts/ci/t81lang_repro_gate.py`.
