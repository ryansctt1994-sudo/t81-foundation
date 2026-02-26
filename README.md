<p align="center">
  <a href="https://github.com/t81dev/t81-foundation/stargazers"><img src="https://img.shields.io/github/stars/t81dev/t81-foundation?style=for-the-badge&logo=github&color=blue" alt="Stars"></a>
  <a href="https://github.com/t81dev/t81-foundation/network/members"><img src="https://img.shields.io/github/forks/t81dev/t81-foundation?style=for-the-badge&logo=github&color=blue" alt="Forks"></a>
  <a href="https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml"><img src="https://img.shields.io/github/actions/workflow/status/t81dev/t81-foundation/ci.yml?branch=main&style=for-the-badge&logo=github" alt="CI Status"></a>
  <a href="https://github.com/t81dev/t81-foundation/commits/main"><img src="https://img.shields.io/github/commit-activity/m/t81dev/t81-foundation?style=for-the-badge&logo=github&color=green" alt="Commit Activity"></a>
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge" alt="License: MIT"></a>
  <a href="https://en.cppreference.com/w/cpp/23"><img src="https://img.shields.io/badge/Language-C%2B%2B23-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="Language: C++23"></a>
</p>

<p align="center">
  <img src="assets/banner.png" alt="T81 Foundation architecture banner">
</p>

<p align="center">
  <strong>Deterministic ternary-native computing stack featuring base-81 data types, TISC instruction set, T81VM, T81Lang, Axion policy engine, and recursive cognition tiers. T81 is a deterministic computing platform with a formally frozen ISA and verifiable reproducibility guarantees for defined surfaces.
</strong>
</p>

---

<p align="center">
  <a href="README.md"><img src="https://img.shields.io/badge/Lang-English-blue?style=flat-square" alt="English"></a>
  <a href="README.zh-CN.md"><img src="https://img.shields.io/badge/Lang-%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-red?style=flat-square" alt="简体中文"></a>
  <a href="README.es.md"><img src="https://img.shields.io/badge/Lang-Espa%C3%B1ol-green?style=flat-square" alt="Español"></a>
  <a href="README.ru.md"><img src="https://img.shields.io/badge/Lang-%D0%A0%D1%83%D1%81%D1%81%D0%BA%D0%B8%D0%B9-blueviolet?style=flat-square" alt="Русский"></a>
  <a href="README.pt-BR.md"><img src="https://img.shields.io/badge/Lang-Portugu%C3%AAs-orange?style=flat-square" alt="Português"></a>
</p>

---

# T81 Foundation - Deterministic Ternary-Based Architecture for Governed AGI

## What T81 Is
T81 is a deterministic computing platform built around balanced ternary and base-81 data representations, a frozen TISC ISA boundary, and governance-enforced reproducibility for verified determinism surfaces. Bit-exact guarantees are scoped to documented verified surfaces and the Deterministic Core Profile (DCP), not to all repository modules.

## What T81 Is Not

- Not a replacement for general-purpose language ecosystems.
- Not a platform that permits fast-math shortcuts on deterministic surfaces.
- Not a claim of determinism outside surfaces marked **Verified** in the registry.
- Not a claim that experimental modules are release-certified deterministic.

---

## Quick Start

```bash
# Configure and build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 1

# Run tests
ctest --test-dir build --output-on-failure -j1

# Verify determinism gate inputs/hashes
python3 scripts/ci/t81lang_repro_gate.py
````

---

## Determinism Scope

Determinism guarantees apply only to surfaces marked **Verified** in the registry.

* The stable, release-certified subset is defined by the Deterministic Core Profile.
* Experimental modules are out of deterministic release guarantees unless explicitly promoted.
* JIT is excluded from deterministic guarantees unless registry status is upgraded to **Verified**.

References:

* [Deterministic Core Profile](docs/product/DETERMINISTIC_CORE_PROFILE.md)
* [Determinism Surface Registry](docs/governance/DETERMINISM_SURFACE_REGISTRY.md)
* [Determinism Threat Model](docs/governance/DETERMINISM_THREAT_MODEL.md)

---

## Architectural Overview

High-level layers:

* **Foundation** — Core types, ISA, VM execution semantics.
* **Kernel** — Axion policy and capability enforcement.
* **Runtime** — Tracing and JIT-related runtime paths.
* **Higher-Level & Research** — Language frontend and experimental tiers.

Architecture reference:
→ [docs/architecture/OVERVIEW.md](docs/architecture/OVERVIEW.md)

Authority note: README is a summary. Normative definitions reside in [`/spec`](spec/) under the authority model.

---

## Repository Structure

* **Core**: `core/types`, `core/isa`, `core/vm`
* **Kernel**: `kernel/axion`
* **Runtime**: `runtime/tracing`, `runtime/jit`
* **Experimental**: `experimental/*`
* **Language & Tooling**: `lang/`, `tooling/`, `tools/`
* **Public API**: `include/t81/**`
* **Specification**: `spec/`
* **Governance**: `docs/governance/`
* **Status Tracking**: `docs/status/`
* **Product Docs**: `docs/product/`
* **Book (non-normative)**: `book/`

**Public API Contract:**
Only headers under `include/t81/**` are supported as stable C++ API surface.

---

## Project Status

* Deterministic Core Profile: Stable
* Experimental Modules: Active development
* Governance Framework: Enforced
* External Packaging: Supported via CMake

---

## Governed AGI Direction

T81 is oriented toward a deterministic ternary-based architecture for governed
AGI workloads, with strict boundary controls:

* Determinism guarantees remain bounded to registry-verified surfaces and DCP.
* AGI-facing and cognitive-tier surfaces are non-DCP unless explicitly
  promoted.
* Promotion to stronger guarantees follows an explicit governance pipeline.

References:

* [Strategic Direction](docs/product/STRATEGIC_DIRECTION.md)
* [Governed AGI Promotion Pipeline](docs/status/GOVERNED_AGI_PROMOTION_PIPELINE.md)

---

## Governance & Enforcement Model

* [Specification Authority Model](docs/governance/SPEC_AUTHORITY_MODEL.md) — Source-of-truth hierarchy and conflict resolution.
* [Freeze Enforcement](docs/governance/FREEZE_ENFORCEMENT.md) — Frozen boundaries and break discipline.
* [Determinism Surface Registry](docs/governance/DETERMINISM_SURFACE_REGISTRY.md) — Enumerated determinism claims and verification status.
* [Determinism Threat Model](docs/governance/DETERMINISM_THREAT_MODEL.md) — Determinism risk classes and mitigations.
* [Incident Response Plan](docs/governance/INCIDENT_RESPONSE.md) — Determinism/security breach handling.
* [Release Discipline Manifest](docs/product/RELEASE_DISCIPLINE.md) — Release preconditions and SemVer discipline.
* [Dependency Firewall](docs/architecture/DEPENDENCY_FIREWALL.md) — Architectural boundary enforcement.

---

## Deterministic Core Profile (DCP)

DCP defines the stable, release-certified deterministic subset of T81.

* Releases certify DCP compliance.
* Deterministic guarantees apply only to DCP-listed verified surfaces.
* Versioning discipline:

  * **MAJOR** — Frozen boundary violation or incompatible change
  * **MINOR** — Backward-compatible feature addition
  * **PATCH** — Bugfix-only scope

Reference:
→ [docs/product/DETERMINISTIC_CORE_PROFILE.md](docs/product/DETERMINISTIC_CORE_PROFILE.md)

---

## Supported Platforms

All listed platforms pass the Determinism Gate for DCP surfaces.

Platform support applies to DCP surfaces only.

| Platform              | Architecture | Status    |
| --------------------- | ------------ | --------- |
| Ubuntu 22.04+         | x86-64       | Supported |
| macOS (Apple Silicon) | ARM64        | Supported |

---

## Evaluating T81

To independently evaluate deterministic guarantees:

1. Review the Determinism Surface Registry.
2. Review the Deterministic Core Profile definition.
3. Run the determinism gate locally.
4. Inspect release evidence under the Release Discipline Manifest.

---

## Contributing

All contributions must align with institutional controls:

* **Spec-first:** Normative behavior changes must be anchored in `/spec`.
* **Freeze compliance:** Frozen boundaries require explicit governance discipline.
* **ADR discipline:** Architectural or governance changes require ADR entries.
* **Determinism discipline:** Determinism-affecting changes must pass determinism gates.
* **Public API boundary:** Changes under `include/t81/**` are compatibility-sensitive.

Contribution guide:
→ [CONTRIBUTING.md](CONTRIBUTING.md)

---

## Non-Goals

* Guaranteeing determinism outside verified surfaces.
* Guaranteeing deterministic wall-clock performance.
* Guaranteeing network timing determinism.
* Treating experimental modules as DCP-certified by default.
* Relaxing frozen ISA/data-type boundaries without formal governance process.

---

## Documentation Map

* **Normative Specifications** — [`/spec`](spec/)
* **Architecture** — [docs/architecture/OVERVIEW.md](docs/architecture/OVERVIEW.md)
* **Governance** — [docs/governance/](docs/governance/)
* **Status** — [docs/status/](docs/status/)
* **Product** — [docs/product/](docs/product/) (includes [Strategic Direction](docs/product/STRATEGIC_DIRECTION.md))

---

## The T81 Details

<details>
<summary><strong>Part I — Foundations</strong></summary>

1. **[Introduction](./01_Introduction.md)**

   * [1.1 Scope and Definition](./01_Introduction.md#11-scope-and-definition)
   * [1.2 System Architecture](./01_Introduction.md#12-system-architecture)
   * [1.3 Verifiable Compute Mission](./01_Introduction.md#13-verifiable-compute-mission)
   * [1.4 Terminology](./01_Introduction.md#14-terminology)
   * [1.5 Verification Checklist](./01_Introduction.md#15-verification-checklist)

2. **[Core Principles and Invariants](./02_Principles.md)**

   * [2.1 The Determinism Invariant](./02_Principles.md#21-the-determinism-invariant)
   * [2.1.1 Determinism Surfaces and Attack Vectors](./02_Principles.md#211-determinism-surfaces-and-attack-vectors)
   * [2.2 Ternary Logic (Base-3)](./02_Principles.md#22-ternary-logic-base-3)
   * [2.3 Auditability and the Axion Trace](./02_Principles.md#23-auditability-and-the-axion-trace)
   * [2.4 The Nine Principles (Ethics Enforcement)](./02_Principles.md#24-the-nine-principles-ethics-enforcement)
   * [2.5 Verification Checklist](./02_Principles.md#25-verification-checklist)
   * [2.6 Formal Audit Matrix](./02_Principles.md#26-formal-audit-matrix)

</details>

<details>
<summary><strong>Part II — The Deterministic Machine</strong></summary>

3. **[T81VM Architecture](./03_Architecture.md)**

   * [3.1 Overview](./03_Architecture.md#31-overview)
   * [3.2 The Runtime Boundary](./03_Architecture.md#32-the-runtime-boundary)
   * [3.3 Memory Model](./03_Architecture.md#33-memory-model)
   * [3.4 The Instruction Set (TISC)](./03_Architecture.md#34-the-instruction-set-tisc)
   * [3.5 JIT Compilation (Trace-JIT)](./03_Architecture.md#35-jit-compilation-trace-jit)

4. **[Data Types and Serialization](./04_Data_Types_and_Serialization.md)**

   * [4.1 Primitive Types](./04_Data_Types_and_Serialization.md#41-primitive-types)
   * [4.2 T81Float and dmath](./04_Data_Types_and_Serialization.md#42-t81float-and-dmath)
   * [4.3 Tensors and Canonical Layouts](./04_Data_Types_and_Serialization.md#43-tensors-and-canonical-layouts)
   * [4.4 Canonical Serialization Rules](./04_Data_Types_and_Serialization.md#44-canonical-serialization-rules)

5. **[Installation and Build Verification](./05_Installation.md)**

   * [5.1 Prerequisites](./05_Installation.md#51-prerequisites)
   * [5.2 Building from Source](./05_Installation.md#52-building-from-source)
   * [5.3 Verifying the Build](./05_Installation.md#53-verifying-the-build)
   * [5.4 Troubleshooting](./05_Installation.md#54-troubleshooting)

6. **[CLI and API Usage](./06_Usage.md)**

   * [6.1 The T81 Command Line Interface](./06_Usage.md#61-the-t81-command-line-interface)
   * [6.2 Embedding T81 (C++ API)](./06_Usage.md#62-embedding-t81-c-api)
   * [6.3 Embedding T81 (Python API)](./06_Usage.md#63-embedding-t81-python-api)
   * [6.4 Debugging](./06_Usage.md#64-debugging)

7. **[Programming in T81Lang](./07_Programming_in_T81Lang.md)**

   * [7.1 Design Philosophy](./07_Programming_in_T81Lang.md#71-design-philosophy)
   * [7.2 Syntax Basics](./07_Programming_in_T81Lang.md#72-syntax-basics)
   * [7.3 Data Types](./07_Programming_in_T81Lang.md#73-data-types)
   * [7.4 Control Flow](./07_Programming_in_T81Lang.md#74-control-flow)
   * [7.5 Functions](./07_Programming_in_T81Lang.md#75-functions)
   * [7.6 Axion Integration](./07_Programming_in_T81Lang.md#76-axion-integration)
   * [7.7 Examples](./07_Programming_in_T81Lang.md#77-examples)

</details>

<details>
<summary><strong>Part III — Governance and Verification</strong></summary>

8. **[Verification and Audit](./08_Verification_and_Audit.md)**

   * [8.1 Formal Verification Methodology](./08_Verification_and_Audit.md#71-formal-verification-methodology)
   * [8.2 The Formal Audit Matrix](./08_Verification_and_Audit.md#72-the-formal-audit-matrix)
   * [8.3 Property-Based Testing](./08_Verification_and_Audit.md#73-property-based-testing)
   * [8.4 The Determinism Gate](./08_Verification_and_Audit.md#74-the-determinism-gate)

9. **[The Axion Safety Kernel](./09_The_Axion_Kernel.md)**

   * [9.1 Formal Definition](./09_The_Axion_Kernel.md#81-formal-definition)
   * [9.2 The Policy Model](./09_The_Axion_Kernel.md#82-the-policy-model)
   * [9.3 Instruction Interception](./09_The_Axion_Kernel.md#83-instruction-interception)
   * [9.4 The Audit Log (Trace)](./09_The_Axion_Kernel.md#84-the-audit-log-trace)
   * [9.5 Cognitive Promotion](./09_The_Axion_Kernel.md#85-cognitive-promotion)

10. **[Cognitive Tiers and Distributed Compute](./10_Cognitive_Tiers_and_Distributed_Compute.md)**

   * [10.1 The Cognitive Tier Model](./10_Cognitive_Tiers_and_Distributed_Compute.md#91-the-cognitive-tier-model)
   * [10.2 Distributed Compute (Tier 4)](./10_Cognitive_Tiers_and_Distributed_Compute.md#92-distributed-compute-tier-4)
   * [10.3 Trace-Based JIT Compilation](./10_Cognitive_Tiers_and_Distributed_Compute.md#93-trace-based-jit-compilation)
   * [10.4 Infinite Forms (Tier 5)](./10_Cognitive_Tiers_and_Distributed_Compute.md#94-infinite-forms-tier-5)

11. **[Appendices](./11_Appendices.md)**

   * [11.1 What Is Not Yet Implemented](./11_Appendices.md#101-what-is-not-yet-implemented)
   * [11.2 Glossary](./11_Appendices.md#102-glossary)
   * [11.3 Useful Links](./11_Appendices.md#103-useful-links)

</details>

<details>
<summary><strong>Part IV — Formalization and Structural Hardening</strong></summary>

12. **[Formal Semantics of TISC and T81VM](./12_Formal_Semantics.md)**

   * [12.1 Operational Semantics](./12_Formal_Semantics.md#111-operational-semantics)
   * [12.2 Algebraic Transition Function](./12_Formal_Semantics.md#112-algebraic-transition-function)
   * [12.3 Canonicalization Rewriting System](./12_Formal_Semantics.md#113-canonicalization-rewriting-system)
   * [12.4 Determinism Proof Sketches](./12_Formal_Semantics.md#114-determinism-proof-sketches)
   * [12.5 Interpreter vs Trace-JIT Equivalence](./12_Formal_Semantics.md#115-interpreter-vs-trace-jit-equivalence)

13. **[Adversarial Modeling and Determinism Attacks](./13_Adversarial_Modeling.md)**

   * [13.1 Threat Model](./13_Adversarial_Modeling.md#121-threat-model)
   * [13.2 Compiler-Level Attacks](./13_Adversarial_Modeling.md#122-compiler-level-attacks)
   * [13.3 VM and GC Attack Vectors](./13_Adversarial_Modeling.md#123-vm-and-gc-attack-vectors)
   * [13.4 CanonFS and Hash Attacks](./13_Adversarial_Modeling.md#124-canonfs-and-hash-attacks)
   * [13.5 Distributed Tier Time-Travel Attack](./13_Adversarial_Modeling.md#125-distributed-tier-time-travel-attack)
   * [13.6 Determinism Breach Postmortem Template](./13_Adversarial_Modeling.md#126-determinism-breach-postmortem-template)

</details>

<details>
<summary><strong>Part V — Continuity and Research Horizon</strong></summary>

14. **[Continuity and Resilience](./14_Continuity_Resilience.md)**

   * [14.1 The Cleanroom Protocol](./14_Continuity_Resilience.md#131-the-cleanroom-protocol)
   * [14.2 Single Points of Failure](./14_Continuity_Resilience.md#132-single-points-of-failure)
   * [14.3 Continuity Manifest](./14_Continuity_Resilience.md#133-continuity-manifest)
   * [14.4 Immutable Formal Invariants](./14_Continuity_Resilience.md#134-immutable-formal-invariants)

15. **[Research Frontier](./15_Research_Frontier.md)**

   * [15.1 Ternary Hardware Acceleration](./15_Research_Frontier.md#141-ternary-hardware-acceleration)
   * [15.2 Formal Verification Paths](./15_Research_Frontier.md#142-formal-verification-paths)
   * [15.3 CanonFS as a Merkle Substrate](./15_Research_Frontier.md#143-canonfs-as-a-merkle-substrate)
   * [15.4 Deterministic AI Inference at Scale](./15_Research_Frontier.md#144-deterministic-ai-inference-at-scale)

</details>

---

## License

MIT License. See [LICENSE](LICENSE).
