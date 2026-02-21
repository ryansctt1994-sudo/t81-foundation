# Chapter 13: Continuity and Resilience

## 13.1 The Cleanroom Protocol

**Status: Documented**

The **Cleanroom Reconstruction Protocol** defines the minimal set of steps required to rebuild the T81 system from scratch, assuming total infrastructure loss.

### 13.1.1 Minimal Bootstrap
1.  **Source Code**: A copy of the `src/` and `include/` directories.
2.  **Compiler**: Any C++23 compliant compiler (Clang 18+, GCC 14+).
3.  **Build System**: CMake 3.25+.

**Command**:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### 13.1.2 Verification
After rebuilding, the system must verify itself against a known set of `CanonHash81` artifacts.
```bash
./build/t81 repro-hash tests/fixtures/t81lang_determinism
```

## 13.2 Long-Term Archival

**Status: Aspirational**

The goal of T81 is to be readable and executable in 50+ years.
*   **Format Stability**: The `.t81` source and `.tisc` bytecode formats are frozen.
*   **Dependencies**: The core VM has zero external runtime dependencies beyond the standard C++ library.
