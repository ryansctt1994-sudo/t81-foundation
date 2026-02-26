<p align="center">
  <a href="https://github.com/t81dev/t81-foundation/releases/latest"><img src="https://img.shields.io/github/v/release/t81dev/t81-foundation?style=for-the-badge&label=Latest%20Release" alt="Latest Release"></a>
  <a href="https://github.com/t81dev/t81-foundation/stargazers"><img src="https://img.shields.io/github/stars/t81dev/t81-foundation?style=for-the-badge&logo=github&color=blue" alt="Stars"></a>
  <a href="https://github.com/t81dev/t81-foundation/network/members"><img src="https://img.shields.io/github/forks/t81dev/t81-foundation?style=for-the-badge&logo=github&color=blue" alt="Forks"></a>
  <a href="https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml"><img src="https://img.shields.io/github/actions/workflow/status/t81dev/t81-foundation/ci.yml?branch=main&style=for-the-badge&logo=github" alt="CI Status"></a>
  <a href="https://github.com/t81dev/t81-foundation/commits/main"><img src="https://img.shields.io/github/commit-activity/m/t81dev/t81-foundation?style=for-the-badge&logo=github&color=green" alt="Commit Activity"></a>
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge" alt="License: MIT"></a>
  <a href="https://en.cppreference.com/w/cpp/23"><img src="https://img.shields.io/badge/Language-C%2B%2B23-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="Language: C++23"></a>
</p>

<p align="center">
  <img src="assets/banner.png" alt="T81 Foundation — Deterministic Ternary Architecture" width="820">
</p>

<p align="center">
  <strong>Deterministic ternary-native computing stack featuring base-81 data types, TISC instruction set, T81VM, T81Lang, Axion safety/optimization engine, and recursive cognition tiers — built for bit-exact, auditable, reproducible execution in AI, cryptography, and scientific computing.</strong>
</p>

<p align="center">
  <a href="README.md"><img src="https://img.shields.io/badge/Lang-English-blue?style=flat-square" alt="English"></a>
  <a href="README.zh-CN.md"><img src="https://img.shields.io/badge/Lang-%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-red?style=flat-square" alt="简体中文"></a>
  <a href="README.es.md"><img src="https://img.shields.io/badge/Lang-Espa%C3%B1ol-green?style=flat-square" alt="Español"></a>
  <a href="README.ru.md"><img src="https://img.shields.io/badge/Lang-%D0%A0%D1%83%D1%81%D1%81%D0%BA%D0%B8%D0%B9-blueviolet?style=flat-square" alt="Русский"></a>
  <a href="README.pt-BR.md"><img src="https://img.shields.io/badge/Lang-Portugu%C3%AAs-orange?style=flat-square" alt="Português"></a>
</p>

---

**T81 Foundation 1.0.0 Sovereign** is live (Feb 22, 2026).  
A sovereign, ternary-first stack that eliminates floating-point non-determinism and delivers **mathematical certainty** for high-stakes workloads.

> 💡 **Why it matters:** In AI safety, financial modeling, cryptography, and scientific computing, “mostly reproducible” is no longer acceptable. T81 guarantees the same bit-exact result on every supported platform, every time.

---

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Quick Start](#quick-start)
- [Supported Platforms](#supported-platforms)
- [CLI Examples](#cli-examples)
- [Live Demos](#live-demos)
- [Repository Map](#repository-map)
- [Document Authority Map](#document-authority-map)
- [Compatibility & Non-Goals](#compatibility--non-goals)
- [Configuration & Axion](#configuration--axion)
- [Contributing](#contributing)
- [Changelog](#changelog)
- [Acknowledgments](#acknowledgments)
- [License](#license)

## Features

| Feature                  | Status       | Description |
|--------------------------|--------------|-------------|
| **Deterministic Execution** | ✨ Stable    | Bit-exact results across x86/ARM/Apple Silicon via `dmath` & custom FP |
| **Ternary-Native Types**    | ✨ Stable    | Base-81 balanced ternary integers & floats (no sign bit, reduced carry) |
| **T81VM & TISC**            | ✨ Stable    | 81-register VM with deterministic interpreter + Trace-JIT |
| **Axion Engine**            | ✨ Stable    | Runtime policy, safety, ethics, optimization & audit traces |
| **Model Tooling**           | ✨ Stable    | SafeTensors / GGUF / T81W import, quantization, tensor handles |
| **Reproducibility Gate**    | ✨ Stable    | CI-enforced `t81lang_repro_gate.py` |
| **Cognitive Tiers**         | 🚧 Beta     | Recursive layers: Symbolic → Distributed → Infinite Forms |
| **Trace-JIT**               | 🚧 Experimental | Hotspot optimization that never breaks determinism |

## Architecture

```mermaid
graph TD
    subgraph "Language Toolchain"
        A["T81Lang (.t81)"] --> B[Lexer]
        B --> C[Parser]
        C --> D[AST]
        D --> E["Semantic Analyzer"]
        E --> F["IR Generator"]
        F --> G["TISC IR"]
        G --> H["Binary Emitter"]
        H --> I["TISC Bytecode"]
    end
    subgraph "Runtime"
        I --> J["T81VM Interpreter"]
        J --> K["Trace Hotspot Detection"]
        K --> L["Deterministic Trace-JIT"]
        L --> M["Compiled Execution"]
    end
    subgraph "Safety & Audit"
        J --> N["Axion Policy Engine"]
        M --> N
        N --> O["Events / Verdicts / Trace"]
    end
    subgraph "Model Tooling"
        P["SafeTensors / GGUF / T81W"] --> Q["Weights Import"]
        Q --> R["Tensor Handles"]
        R --> J
    end
```

## Quick Start (under 60 seconds)

```bash
# 1. Clone & build
git clone https://github.com/t81dev/t81-foundation.git
cd t81-foundation
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# 2. Compile & run Hello World
./build/t81 compile examples/hello_world.t81 -o hello.tisc
./build/t81 run hello.tisc

# 3. Verify determinism (mandatory for any release)
python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
# ✅  All determinism checks passed.
```

Works instantly in the included **.devcontainer/** (VS Code / GitHub Codespaces ready).

## Supported Platforms

All platforms pass the Determinism Gate with identical output hashes.

| Platform          | Arch       | Compiler          | Status |
|-------------------|------------|-------------------|--------|
| **Linux**         | x86_64     | Clang 18+, GCC 14+| ✅ Verified |
| **Linux**         | ARM64      | Clang 18+         | ✅ Verified |
| **macOS**         | Intel      | Apple Clang       | ✅ Verified |
| **macOS**         | Apple Silicon | Apple Clang    | ✅ Verified |
| **Devcontainer / Codespaces** | Any     | Included config   | ✅ Instant |

## CLI Examples

```bash
# Development
t81 compile src.t81 -o out.tisc
t81 run out.tisc
t81 disasm out.tisc

# Debugging & Audit
t81 debug out.tisc
t81 trace show trace.txt
t81 repro-hash tests/fixtures/

# AI / Tensors
t81 weights import model.safetensors -o model.t81w
t81 weights quantize model.safetensors --to-gguf model.gguf
```

## Live Demos

After building, try these built-in binaries:

```bash
./build/t81_demo           # BigInt + Tensor showcase
./build/t81_tensor_ops     # Tensor transpose, slice, dot product
./build/llama-run          # Governed Llama.cpp adapter (latest addition)
```

## Repository Map

- **`assets/`** — banner & visuals  
- **`book/book-en/`** — The Definitive Technical Monograph  
- **`core/`, `kernel/`, `lang/`, `runtime/`** — Modular source  
- **`examples/`** — `hello_world.t81`, tensor demos, governance examples  
- **`include/t81/`** — Public C++ API  
- **`scripts/ci/`** — Reproducibility gate & CI  
- **`spec/`** — Normative specifications  
- **`docs/governance/`** — Determinism Surface Registry  

## Document Authority Map

The **Definitive Technical Monograph** (`book/book-en/`) is the single source of truth.

<details>
<summary><strong>Part I — Foundations</strong></summary>

1. [Introduction](book/book-en/01_Introduction.md)  
2. [Core Principles and Invariants](book/book-en/02_Principles.md)

</details>

<details>
<summary><strong>Part II — The Deterministic Machine</strong></summary>

3. [T81VM Architecture](book/book-en/03_Architecture.md)  
4. [Data Types and Canonical Serialization](book/book-en/04_Data_Types_and_Serialization.md)  
5. [Installation and Build Verification](book/book-en/05_Installation.md)  
6. [CLI and API Usage](book/book-en/06_Usage.md)  
7. [Programming in T81Lang](book/book-en/07_Programming_in_T81Lang.md)

</details>

<details>
<summary><strong>Part III — Governance and Verification</strong></summary>

8. [Verification and Audit](book/book-en/08_Verification_and_Audit.md)  
9. [The Axion Safety Kernel](book/book-en/09_The_Axion_Kernel.md)  
10. [Cognitive Tiers and Distributed Compute](book/book-en/10_Cognitive_Tiers_and_Distributed_Compute.md)

</details>

<details>
<summary><strong>Part IV — Formalization and Structural Hardening</strong></summary>

12. [Formal Semantics of TISC and T81VM](book/book-en/12_Formal_Semantics.md)  
13. [Adversarial Modeling and Determinism Attacks](book/book-en/13_Adversarial_Modeling.md)

</details>

<details>
<summary><strong>Part V — Continuity and Research Horizon</strong></summary>

14. [Continuity and Resilience](book/book-en/14_Continuity_Resilience.md)  
15. [Research Frontier](book/book-en/15_Research_Frontier.md)

</details>

> 📚 **Read the full monograph here:** [book/book-en/README.md](book/book-en/README.md)

## Compatibility & Non-Goals

**Guarantees**  
- TISC bytecode forward-compatible within major versions  
- Determinism is a non-negotiable invariant (breaking it = critical security bug)

**Non-Goals**  
- Raw speed at the expense of reproducibility  
- General-purpose replacement for Python/C++/Rust

## Configuration & Axion

Axion enforces runtime policies (safety, ethics, optimization) via config files or flags.  
See `examples/axion_policy_runner` and the full chapter in the book.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). All determinism-affecting changes must pass the repro gate.

## Changelog

- **v1.0.0-Sovereign** (Feb 22, 2026) — First production-ready release. Stable VM, Axion kernel, Nine Principles enforcement, multilingual monograph.

Full history → [Releases](https://github.com/t81dev/t81-foundation/releases)

## Acknowledgments

Thanks to the open-source community, LLVM, fmt, and early ternary computing researchers.

## License

MIT License — see [LICENSE](LICENSE).

---

**T81 1.0.0 Sovereign is ready.**  
If you want systems that will still be auditable and governed ten years from now, this is the foundation.

Start with the Quick Start above, then open the monograph.

We’d love to see what you build.

— The T81 Team
