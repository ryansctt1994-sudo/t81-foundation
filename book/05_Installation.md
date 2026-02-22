# Chapter 5: Installation and Build Verification

## 5.1 Prerequisites

**Status: Standardized**

Building T81 requires a modern C++ toolchain capable of supporting C++23 features. The project enforces strict compiler warnings and standard compliance to minimize undefined behavior.

### Supported Platforms
*   **Linux**: x86_64, ARM64 (aarch64), RISC-V (rv64gc)
*   **macOS**: Apple Silicon (M1/M2/M3), Intel (legacy)
*   **Windows**: WSL2 recommended (MSVC support is experimental)

### Toolchain Requirements
*   **Compiler**:
    *   Clang 18+ (Recommended for strictness)
    *   GCC 14+
    *   MSVC 19.38+ (VS 2022)
*   **Build System**: CMake 3.25 or newer.
*   **Python**: Python 3.10+ (Used for validation scripts and bindings).
*   **Ninja**: Recommended for faster builds.

## 5.2 Building from Source

**Status: Automated**

The standard build process is encapsulated in the `make cmake-ritual` command, but can be manually executed via CMake.

### 5.2.1 The CMake Workflow

```bash
# 1. Clone the repository
git clone https://github.com/t81-foundation/t81.git
cd t81

# 2. Configure (Release mode recommended for performance)
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DT81_USE_CXX23=ON \
    -DT81_BUILD_TESTS=ON

# 3. Build the core executable
cmake --build build --target t81
```

### 5.2.2 Build Options
The following CMake options control the build configuration:

| Option | Default | Description |
| :--- | :--- | :--- |
| `T81_USE_CXX23` | `ON` | Enable C++23 features (e.g., `std::expected`, `std::print`). |
| `T81_BUILD_TESTS` | `ON` | Compile the verification suite (`t81_*_test`). |
| `T81_BUILD_EXAMPLES` | `ON` | Compile demo programs in `examples/`. |
| `T81_ENABLE_ASAN` | `OFF` | Enable AddressSanitizer (Debug only). |
| `T81_ENABLE_UBSAN` | `OFF` | Enable UndefinedBehaviorSanitizer. |

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

*   **"C++23 not supported"**: Upgrade your compiler. T81 relies heavily on modern C++ features for type safety.
*   **"Trace Hash Mismatch"**: You may be linking against a different version of standard libraries, or `dmath` fallback was triggered. Ensure `T81_DETERMINISTIC` is defined.
*   **"SIMD Instruction Fault"**: T81 attempts to detect AVX2/NEON availability. If cross-compiling, ensure target flags are correct.
