# T81 Foundation

<p align="center">
  <strong>Deterministic ternary-native computing stack featuring base-81 data types, TISC instruction set, T81VM, T81Lang, Axion policy engine, and recursive cognition tiers — built for bit-exact, auditable execution across supported platforms.</strong>
</p>

<p align="center">
  <a href="https://github.com/t81dev/t81-foundation/stargazers"><img src="https://img.shields.io/github/stars/t81dev/t81-foundation?style=for-the-badge&logo=github&color=blue" alt="Stars"></a>
  <a href="https://github.com/t81dev/t81-foundation/network/members"><img src="https://img.shields.io/github/forks/t81dev/t81-foundation?style=for-the-badge&logo=github&color=blue" alt="Forks"></a>
  <a href="https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml"><img src="https://img.shields.io/github/actions/workflow/status/t81dev/t81-foundation/ci.yml?branch=main&style=for-the-badge&logo=github" alt="CI Status"></a>
  <a href="https://github.com/t81dev/t81-foundation/commits/main"><img src="https://img.shields.io/github/commit-activity/m/t81dev/t81-foundation?style=for-the-badge&logo=github&color=green" alt="Commit Activity"></a>
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge" alt="License: MIT"></a>
  <a href="https://en.cppreference.com/w/cpp/23"><img src="https://img.shields.io/badge/Language-C%2B%2B23-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="Language: C++23"></a>
</p>

---

T81 is a deterministic computing stack designed to eliminate floating-point nondeterminism and enable auditable execution. By leveraging **balanced ternary logic** and **base-81 data types**, T81 provides **bit-exact reproducibility for verified surfaces** across supported architectures (x86/ARM, macOS/Linux).

> 💡 In safety-critical domains such as AI, finance, and cryptography, reproducibility is a prerequisite for trust. T81 prioritizes deterministic behavior where formally defined and verified.

---

## Authority

This project follows a strict [Specification Authority Model](docs/governance/SPEC_AUTHORITY_MODEL.md).

* **[Architecture Overview](docs/architecture/OVERVIEW.md)** — System boundaries and invariants.
* **[`/spec/`](spec/)** — Normative technical definitions.
* **[Authority Model](docs/governance/SPEC_AUTHORITY_MODEL.md)** — Conflict resolution and freeze rules.

In case of conflict:

* `/spec/` supersedes `/docs/`
* `docs/architecture/OVERVIEW.md` supersedes README summaries
* `/docs/` supersedes `/book/`

---

## Features

| Feature                     | Status          | Description                                                        |
| :-------------------------- | :-------------- | :----------------------------------------------------------------- |
| **Deterministic Execution** | ✨ Stable        | Bit-exact results for verified VM and data type surfaces.          |
| **Ternary-Native Types**    | ✨ Stable        | Base-81 balanced ternary integers & deterministic soft-float math. |
| **T81VM & TISC**            | ✨ Stable        | Deterministic interpreter for frozen TISC ISA.                     |
| **Axion Engine**            | 🚧 Alpha        | Policy enforcement and trace instrumentation (partial coverage).   |
| **Model Tooling**           | ✨ Stable        | Import/inspect SafeTensors, GGUF, T81W.                            |
| **Reproducibility Gate**    | ✨ Stable        | CI-enforced compiler reproducibility checks.                       |
| **Cognitive Tiers**         | 🚧 Experimental | Recursive execution layers (spec-defined, partially implemented).  |
| **Trace-JIT**               | 🚧 Experimental | Determinism-preserving JIT (spec-defined, not fully verified).     |
| **Multilingual Docs**       | 📚 Live         | Documentation available in multiple languages.                     |

---

## Repository Map

Key directories:

* **`src/`** — Core C++ implementation (VM, Axion, TISC).
* **`include/t81/`** — Public headers.
* **`spec/`** — Normative technical specifications (authoritative).
* **`docs/`** — Architecture, governance, status tracking.
* **`book/`** — Narrative technical monograph (non-normative).
* **`scripts/ci/`** — Determinism and verification gates.
* **`tests/`** — Unit and integration test suite.
* **`examples/`** — Sample programs.
* **`tools/`** — Utilities.
* **`artifacts/`** — Generated outputs (ignored from version control).

---

## Compatibility & Determinism Scope

### Guarantees

* **TISC ISA**: Frozen within major versions.
* **Verified Determinism Surfaces**: Bit-exact behavior where explicitly tested and gated.
* **Compiler Reproducibility**: Enforced via reproducibility scripts in CI.

Determinism surfaces and their verification status are formally enumerated in
[DETERMINISM_SURFACE_REGISTRY.md](docs/governance/DETERMINISM_SURFACE_REGISTRY.md).

Determinism threat analysis is documented in
[DETERMINISM_THREAT_MODEL.md](docs/governance/DETERMINISM_THREAT_MODEL.md).

### Freeze Enforcement

Frozen components (ISA, Data Types, Determinism surfaces) are governed by
`docs/governance/FREEZE_ENFORCEMENT.md`.

Breaking these boundaries requires a MAJOR version bump and formal change record.

### Clarification

Determinism guarantees apply to **verified surfaces only**, as documented in `docs/architecture/OVERVIEW.md`.

Breaking determinism on a verified surface is treated as a critical defect and must not be merged unresolved.

### Non-Goals

* Sacrificing determinism for hardware-specific fast-math optimizations.
* Serving as a general-purpose replacement for C++ or Python.

---

## The Narrative Technical Monograph

The extended technical exposition for T81 is maintained under:

📚 **`/book/`**

The Book provides formal explanations, research context, and extended proofs.
It is descriptive and educational, not normative.

Read it here:
→ [book/book-en/README.md](book/book-en/README.md)

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

All contributions must:

* Align with a normative spec in `/spec/`
* Respect freeze boundaries
* Pass determinism gates where applicable

---

## License

MIT License. See [LICENSE](LICENSE).
