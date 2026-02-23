# Chapter 5: Installation and Build Verification

## 5.1 Prerequisites

**Status: Stable**

To build T81 from source, you need a modern C++ toolchain capable of C++23.

*   **Compiler**:
    *   Clang 16+ (Recommended for strict compliance)
    *   GCC 13+
    *   MSVC 19.36+ (VS 2022)
*   **Build System**:
    *   CMake 3.25+
    *   Ninja (Optional, recommended for speed)
*   **Dependencies**:
    *   Python 3.10+ (For validation scripts)
    *   Git (For version control)

### 5.1.1 Environment Setup (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build clang-16 python3 python3-pip git
```

### 5.1.2 Environment Setup (macOS)
```bash
brew install cmake ninja llvm python3
```

## 5.2 Building from Source

**Status: Implemented**

Clone the repository recursively to fetch submodules (if any).

```bash
git clone https://github.com/t81-foundation/t81.git
cd t81
```

### 5.2.1 Standard Release Build
This builds the `t81` binary with optimizations (`-O3`) enabled.

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### 5.2.2 Build Options
You can control the build configuration:

| Option | Default | Description |
| :--- | :--- | :--- |
| `T81_USE_CXX23` | `ON` | Enable C++23 features (e.g., `std::expected`, `std::print`). |
| `T81_BUILD_TESTS` | `ON` | Compile the verification suite (`t81_*_test`). |
| `T81_BUILD_EXAMPLES` | `ON` | Compile demo programs in `examples/`. |
| `T81_ENABLE_ASAN` | `OFF` | Enable AddressSanitizer (Debug only). |
| `T81_ENABLE_UBSAN` | `OFF` | Enable UndefinedBehaviorSanitizer. |
| `T81_DETERMINISTIC` | `AUTO` | Force usage of `dmath` over `libm`. If `AUTO`, attempts to detect if `libm` is safe (rarely true). |

> **Note on Determinism**: To ensure strict determinism (disabling host FPU fallbacks for transcendentals), define `T81_DETERMINISTIC` manually if not set by default:
> `cmake -B build -DCMAKE_CXX_FLAGS="-DT81_DETERMINISTIC"`

## 5.3 Verifying the Build

**Status: Critical**

After building, you **must** verify that the binary produced is compliant with the T81 spec. A successful compile does not guarantee correct execution.

### 5.3.1 Running Unit Tests
Execute the standard test suite via `ctest`. This runs hundreds of property-based tests.

```bash
cd build
ctest --output-on-failure
```

### 5.3.2 The Determinism Gate
The most critical check is the **Determinism Gate**. This script compiles a canonical reference program, runs it, and compares the resulting Axion Trace hash against a known-good value.

```bash
# Run the repro gate
python3 scripts/ci/t81lang_repro_gate.py --binary ./build/t81
```

**Expected Output**:
```text
[PASS] Trace Hash: canon:sha3:a7f92b... MATCHES expected baseline.
[PASS] Cycles: 10420 (Exact match)
[PASS] Determinism verification successful.
```

If this script fails, the build is **tainted** and must not be used for production or auditing tasks.

### 5.3.3 Verifying Architecture Targets
Ensure that the build graph matches the architectural specification:

```bash
python3 scripts/ci/check_architecture_targets.py
```

## 5.4 Troubleshooting

*   **"C++23 not supported"**: Upgrade your compiler. T81 relies heavily on modern C++ features for type safety (`std::expected`, `std::span`). Check `clang --version`.
*   **"Trace Hash Mismatch"**: You may be linking against a different version of standard libraries, or `dmath` fallback was triggered. Ensure `T81_DETERMINISTIC` is defined. Check for compiler flags like `-ffast-math` which break IEEE compliance—T81 forbids them.
*   **"SIMD Instruction Fault"**: T81 attempts to detect AVX2/NEON availability. If cross-compiling, ensure target flags are correct. Use `-DENABLE_AVX2=OFF` if on older hardware.
*   **"Memory Allocation Failed"**: If running large models, ensure your ulimit is sufficient. T81 aggressively pre-allocates tensor pools.
