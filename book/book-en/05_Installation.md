# Chapter 5: Installation and Build Verification

## 5.1 Prerequisites

**Status: Current**

To build T81 from source, use a modern C++ toolchain and the current repository defaults.

*   **Compiler**:
    *   Clang 18+ (recommended)
    *   GCC 14+
*   **Build System**:
    *   CMake 3.21+
    *   Ninja (Optional, recommended for speed)
*   **Dependencies**:
    *   Python 3.10+ (for determinism gates and CI scripts)
    *   Git (For version control)

### 5.1.1 Environment Setup (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build clang python3 python3-pip git
```

### 5.1.2 Environment Setup (macOS)
```bash
brew install cmake ninja llvm python3
```

## 5.2 Building from Source

**Status: Implemented**

Clone the main repository:

```bash
git clone https://github.com/t81dev/t81-foundation.git
cd t81-foundation
```

### 5.2.1 Standard Release Build
This builds the `t81` binary in release mode.

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### 5.2.2 Build Options
You can control the build configuration:

| Option | Default | Description |
| :--- | :--- | :--- |
| `T81_USE_CXX23` | `ON` | Build in C++23 mode (falls back to C++20 if disabled). |
| `T81_BUILD_TESTS` | `ON` | Build C++ test targets. |
| `T81_BUILD_EXAMPLES` | `ON` | Build examples in `examples/`. |
| `T81_BUILD_BENCHMARKS` | `ON` | Build benchmark suite. |
| `T81_ENABLE_ASAN` | `OFF` | Enable AddressSanitizer where supported. |
| `T81_ENABLE_UBSAN` | `OFF` | Enable UndefinedBehaviorSanitizer where supported. |
| `T81_ENABLE_LLAMA_CPP` | `OFF` | Enable governed `llama.cpp` adapter (`llama-run`, experimental non-DCP). |

## 5.3 Verifying the Build

**Status: Critical**

After building, you **must** verify that the binary produced is compliant with the T81 spec. A successful compile does not guarantee correct execution.

### 5.3.1 Running Unit Tests
Execute the standard test suite via `ctest`.

```bash
ctest --test-dir build --output-on-failure -j1
```

### 5.3.2 The Determinism Gate
The reproducibility gate verifies canonical fixture outputs and aggregate hash stability.

```bash
python3 scripts/ci/t81lang_repro_gate.py \
  --t81-bin ./build/t81 \
  --fixtures-dir tests/fixtures/t81lang_determinism \
  --workdir build/t81lang-repro-check \
  --hash-out build/t81lang-repro-check/hash.txt \
  --expected-hash-file tests/fixtures/t81lang_determinism/t81lang_repro_hash.txt
```

If this gate fails, treat the build as non-release-ready for deterministic claims.

### 5.3.3 Verifying Architecture Targets
Ensure that the build graph matches the architectural specification:

```bash
python3 scripts/ci/check_architecture_targets.py
```

For DCP release checks, also run:

```bash
python3 scripts/ci/check_tisc_freeze_integrity.py
scripts/ci/run_determinism_slice.sh
```

## 5.4 Troubleshooting

*   **"C++23 not supported"**: Upgrade compiler/toolchain and reconfigure from a clean build directory.
*   **"Gate hash mismatch"**: Re-run with a clean build and verify no unsupported optimization flags are injected.
*   **"llama-run unavailable"**: Reconfigure with `-DT81_ENABLE_LLAMA_CPP=ON`.
*   **"Policy required for llama-run"**: `--policy <policy.apl>` is mandatory for governed inference mode.

<!-- chapter-nav-start -->

---

**Navigation**

- [Book Index](./README.md)
- [Previous: Chapter 4: Data Types and Serialization](./04_Data_Types_and_Serialization.md)
- [Next: Chapter 6: CLI and API Usage](./06_Usage.md)

<!-- chapter-nav-end -->
