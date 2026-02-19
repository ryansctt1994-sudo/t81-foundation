# Reproducibility Guide

> **Source of Truth:** This document provides the exact instructions to **reproduce deterministic build artifacts** and verify them against our published hashes.

**Last Updated:** February 10, 2026

## 1. Goal

We guarantee that our build process is deterministic: given the same source, compiler, and build configuration, the output binary will be bit-identical.

## 2. Supported Environment

To reproduce canonical artifacts, you must use the following environment:

| Component | Version | Notes |
| :--- | :--- | :--- |
| **OS** | Ubuntu 24.04 LTS (x86_64 or ARM64) | Standard GitHub Runner Image |
| **Compiler** | Clang 18 | `clang-18`, `clang++-18` |
| **CMake** | 3.28+ | |
| **Build Type** | Release | `-DCMAKE_BUILD_TYPE=Release` |

## 3. Reproduction Ritual (Canonical)

Use the provided CMake preset for exact flag alignment.

```bash
# 1. Clean build directory
rm -rf build

# 2. Configure with CI preset
cmake --preset ci -DCMAKE_CXX_COMPILER=clang++-18 -DCMAKE_C_COMPILER=clang-18

# 3. Build
cmake --build build --parallel

# 4. Verify T81Lang Determinism Hash
python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
```

## 4. Expected Artifact Hashes

For **v1.1.0** (check `git tag`):

| Artifact | Canonical SHA256 (Example) |
| :--- | :--- |
| `t81` Binary (Linux x86_64) | `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` |
| `hello.tisc` (from `hello_world.t81`) | `f2ca1bb6c7e907d06dafe4687e579fce76b37e4e93b7605022da52e6ccc26fd2` |

*(Note: Actual hashes vary by commit. Check CI logs for the latest authoritative hash.)*

## 5. Known Nondeterminism Surfaces

We actively mitigate the following sources of nondeterminism:

- **Timestamps:** We strip build timestamps from binaries where possible or use `SOURCE_DATE_EPOCH`.
- **File Order:** We sort source file lists in `CMakeLists.txt` and `t81 compile` inputs.
- **Concurrency:** Parallel builds (`--parallel`) must not affect output linking order determinism.
- **ASLR/mmap:** JIT compilation uses deterministic memory mapping strategies (see `spec/vm/jit-determinism.md`).

If you find a nondeterministic result, please file a [Bug Report](../../.github/ISSUE_TEMPLATE/bug_report.md).
