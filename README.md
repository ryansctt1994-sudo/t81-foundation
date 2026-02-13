

# T81 Foundation

[![CI](https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml/badge.svg)](https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml)
[![Determinism Gate](https://img.shields.io/badge/Determinism%20Gate-Passing-success)](https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)

[![English](https://img.shields.io/badge/Language-English-blue?style=flat-square)](/README.md)
[![简体中文](https://img.shields.io/badge/Language-%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-red?style=flat-square)](/README.zh-CN.md)
[![Español](https://img.shields.io/badge/Language-Español-green?style=flat-square)](/README.es.md)
[![Русский](https://img.shields.io/badge/Language-Русский-brightgreen?style=flat-square)](/README.ru.md)
[![Português](https://img.shields.io/badge/Language-Português%20(Brasil)-blueviolet?style=flat-square)](/README.pt-BR.md)

---

**Deterministic, governed runtime stack for auditable computing.**

T81 is a deterministic compilation and execution pipeline (`T81Lang -> TISC -> HanoiVM`). It prioritizes auditability, policy enforcement (Axion), and reproducibility over raw hardware speed.

> **Note on Floating Point Determinism:** `T81Float` arithmetic operations involving division and transcendental functions (`sin`, `cos`, `log`, etc.) currently rely on host-platform `double` precision behavior. While `T81Float` *storage* and *canonical serialization* are deterministic, runtime arithmetic results for these specific operations may vary by platform. Strict bit-exact determinism is currently guaranteed for `T81Int`, `T81BigInt`, `T81Fraction` (canonical), and `T81Float` addition/subtraction/multiplication.

## ⚡ 30-Second Evaluation

Verify the claims yourself in 4 steps:

1.  **Build & Run Hello World**
    ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel
    ./build/t81 compile examples/hello_world.t81 -o hello.tisc
    ./build/t81 run hello.tisc
    ```

2.  **Run Determinism Gate**
    ```bash
    # Verify cross-architecture reproducibility hash
    python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
    ```

3.  **Run a VM Demo**
    ```bash
    ./build/t81_demo
    ```

4.  **Inspect a Trace Artifact**
    ```bash
    ./build/t81 trace show trace.txt
    ```

---

## 🚫 Non-Goals

To save your time, here is what T81 is **NOT**:

*   **NOT a hardware accelerator:** T81 does not claim ternary hardware speedups. This is a software runtime for deterministic correctness.
*   **NOT a general-purpose replacement:** T81 focuses on high-stakes, auditable logic, not replacing C++ or Python for general tasks.
*   **NOT "fast and loose":** If a performance optimization breaks trace determinism, T81 rejects it.

---

## ❓ Why This Exists

Modern runtimes trade reproducibility for speed. T81 inverts this: **Auditability is the primary constraint.**

This is enforced via a strict architectural boundary between the Language/Compiler and the Execution Runtime, governed by explicit contracts.

[**View Architectural Boundary Diagram**](ARCHITECTURE.md#3-concurrent-workstream-view) | [**View Runtime Contract**](contracts/runtime-contract.json)

---

## 📚 Document Authority Map

| Document | Purpose | Authority Scope |
| :--- | :--- | :--- |
| **[STATUS.md](STATUS.md)** | What is true *today* | Operational Truth |
| **[ROADMAP.md](ROADMAP.md)** | Forward plan | Strategic |
| **[VERSIONING.md](VERSIONING.md)** | Compatibility rules | Normative |
| **[spec/](spec/)** | Behavioral definition | Normative |
| **[docs/EVIDENCE.md](docs/EVIDENCE.md)** | Proof of claims | Verification |

---

## 🤝 Compatibility Guarantees

*   **Stable:** T81Lang Syntax, TISC Binary Format, HanoiVM Execution Semantics.
*   **Experimental:** JIT Compilation, Distributed Tensor Ops.
*   **SemVer:** T81 follows Semantic Versioning. Breaking changes to **Stable** components increment the Major version.

---

## 🖥️ Supported Platforms

| Platform | Compiler | Status |
| :--- | :--- | :--- |
| **Linux (x86_64)** | Clang 18+, GCC 14+ | ✅ Determinism Gate |
| **Linux (ARM64)** | Clang 18+ | ✅ Determinism Gate |
| **macOS (ARM64)** | Apple Clang | ✅ Supported |

---

## Quick Start (Full)

```bash
git clone https://github.com/t81dev/t81-foundation.git
cd t81-foundation
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Single-threaded safe mode:
```bash
cmake --build build --parallel 1
ctest --test-dir build --output-on-failure -j1
```

## CLI Surface
Common workflows:
```bash
# Compile / run
t81 compile examples/hello_world.t81 -o build/hello.tisc
t81 run build/hello.tisc

# Inspect / debug
t81 disasm build/hello.tisc
t81 debug build/hello.tisc

# Diagnostics / reproducibility
t81 check examples/hello_world.t81
t81 repro-hash tests/fixtures/t81lang_determinism

# Trace workflows
t81 trace show trace.txt
t81 trace diff trace_a.txt trace_b.txt
t81 trace replay build/hello.tisc trace.txt
```

Model tooling:
```bash
t81 weights import model.safetensors -o model.t81w
t81 weights info model.t81w
t81 weights quantize model.safetensors --to-gguf model.gguf
```

See full command help:
```bash
t81 help
```

## Repository Map
- [`include/t81/`](include/t81/): public API headers
- [`src/`](src/): frontend, TISC, VM, Axion, CanonFS, CLI implementation
- [`tests/`](tests/): conformance, determinism, VM/e2e, property slices
- [`docs/`](docs/): guides, status, benchmarks, runtime boundary docs
- [`spec/`](spec/): normative semantics and governance inputs
- [`examples/`](examples/): runnable samples and demos

## Runtime Boundary
T81 uses an explicit runtime boundary contract:
- Marker: [`contracts/runtime-contract.json`](contracts/runtime-contract.json)
- Boundary policy: [`docs/runtime-semantics-boundary.md`](docs/runtime-semantics-boundary.md)

## Further Reading
- [`ARCHITECTURE.md`](ARCHITECTURE.md)
- [`docs/system-integration.md`](docs/system-integration.md)
- [`ANALYSIS.md`](ANALYSIS.md)
- [`CHANGELOG.md`](CHANGELOG.md)
- [`docs/research-guide.md`](docs/research-guide.md)
- [`docs/ai-quickstart.md`](docs/ai-quickstart.md)

## License
This repository is licensed under MIT (see [`LICENSE`](LICENSE)).
