# T81 Foundation

<p align="center">
  <strong>Deterministic ternary-native computing stack featuring base-81 data types, TISC instruction set, T81VM, T81Lang, Axion policy engine, and recursive cognition tiers.</strong>
</p>

<p align="center">
  <a href="https://github.com/t81dev/t81-foundation/stargazers"><img src="https://img.shields.io/github/stars/t81dev/t81-foundation?style=for-the-badge&logo=github&color=blue" alt="Stars"></a>
  <a href="https://github.com/t81dev/t81-foundation/network/members"><img src="https://img.shields.io/github/forks/t81dev/t81-foundation?style=for-the-badge&logo=github&color=blue" alt="Forks"></a>
  <a href="https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml"><img src="https://img.shields.io/github/actions/workflow/status/t81dev/t81-foundation/ci.yml?branch=main&style=for-the-badge&logo=github" alt="CI Status"></a>
  <a href="https://github.com/t81dev/t81-foundation/commits/main"><img src="https://img.shields.io/github/commit-activity/m/t81dev/t81-foundation?style=for-the-badge&logo=github&color=green" alt="Commit Activity"></a>
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge" alt="License: MIT"></a>
  <a href="https://en.cppreference.com/w/cpp/23"><img src="https://img.shields.io/badge/Language-C%2B%2B23-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="Language: C++23"></a>
</p>

## Languages

- [English](README.md)
- [Español](README.es.md)
- [Português (Brasil)](README.pt-BR.md)
- [Русский](README.ru.md)
- [简体中文](README.zh-CN.md)

## What T81 Is

T81 is a deterministic computing platform built around balanced ternary and base-81 data representations, a frozen TISC ISA boundary, and governance-enforced reproducibility for verified determinism surfaces. Bit-exact guarantees are scoped to documented verified surfaces and the Deterministic Core Profile (DCP), not to all repository modules.

## What T81 Is Not

- Not a replacement for general-purpose language ecosystems.
- Not a platform that permits fast-math shortcuts on deterministic surfaces.
- Not a claim of determinism outside surfaces marked **Verified** in the registry.
- Not a claim that experimental modules are release-certified deterministic.

## Quick Start

```bash
# Configure and build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 1

# Run tests
ctest --test-dir build --output-on-failure -j1

# Verify determinism gate inputs/hashes
python3 scripts/ci/t81lang_repro_gate.py
```

## Determinism Scope

Determinism guarantees apply only to surfaces marked **Verified** in the registry.

- The stable, release-certified subset is defined by the Deterministic Core Profile.
- Experimental modules are out of deterministic release guarantees unless explicitly promoted.
- JIT is excluded from deterministic guarantees unless registry status is upgraded to Verified.

References:

- [Deterministic Core Profile](docs/product/DETERMINISTIC_CORE_PROFILE.md)
- [Determinism Surface Registry](docs/governance/DETERMINISM_SURFACE_REGISTRY.md)
- [Determinism Threat Model](docs/governance/DETERMINISM_THREAT_MODEL.md)

## Architectural Overview

High-level layers:

- Foundation: core types, ISA, VM execution semantics.
- Kernel: Axion policy and capability enforcement surfaces.
- Runtime: tracing and JIT-related runtime paths.
- Higher-level and research surfaces: language frontend, experimental tiers, distributed modules.

Architecture reference: [docs/architecture/OVERVIEW.md](docs/architecture/OVERVIEW.md)

Authority note: README is a summary. Normative definitions are in [`/spec`](spec/) under the authority model.

## Repository Structure

- **Core**: `core/types`, `core/isa`, `core/vm`
- **Kernel**: `kernel/axion`
- **Runtime**: `runtime/tracing`, `runtime/jit`
- **Experimental**: `experimental/*` (tiers, hanoi, distributed and related research surfaces)
- **Language & Tooling**: `lang/`, `tooling/`, `tools/`
- **Public API**: `include/t81/**`
- **Governance**: `docs/governance/`
- **Specification**: `spec/`
- **Book (narrative, non-normative)**: `book/`

Public API contract: only headers under `include/t81/**` are supported as stable API surface.

## Governance & Enforcement Model

- [Specification Authority Model](docs/governance/SPEC_AUTHORITY_MODEL.md): defines source-of-truth hierarchy and conflict resolution.
- [Freeze Enforcement](docs/governance/FREEZE_ENFORCEMENT.md): defines frozen boundaries and break discipline.
- [Determinism Surface Registry](docs/governance/DETERMINISM_SURFACE_REGISTRY.md): enumerates deterministic claims and verification status.
- [Determinism Threat Model](docs/governance/DETERMINISM_THREAT_MODEL.md): documents determinism risk classes and mitigations.
- [Incident Response Plan](docs/governance/INCIDENT_RESPONSE.md): defines severity model and determinism/security incident handling.
- [Release Discipline Manifest](docs/product/RELEASE_DISCIPLINE.md): defines release preconditions, SemVer discipline, and release evidence.
- [Dependency Firewall](docs/architecture/DEPENDENCY_FIREWALL.md): defines dependency-boundary controls for architecture integrity.

## Deterministic Core Profile (DCP)

DCP is the product boundary for stable deterministic guarantees and release certification.

- DCP conformance is defined in product and governance docs.
- Release classification follows SemVer discipline aligned with freeze enforcement:
  - MAJOR: frozen boundary violation or incompatible change
  - MINOR: backward-compatible feature addition
  - PATCH: bugfix-only release scope

Reference: [docs/product/DETERMINISTIC_CORE_PROFILE.md](docs/product/DETERMINISTIC_CORE_PROFILE.md)

## Supported Platforms

All listed platforms pass Determinism Gate for DCP surfaces.

| Platform | Architecture | Status |
| :--- | :--- | :--- |
| Ubuntu 22.04+ | x86-64 | Supported |
| macOS (Apple Silicon) | ARM64 | Supported |

## Contributing

Contributions must satisfy institutional controls:

- Spec-first: normative behavior changes must be anchored in `/spec`.
- Freeze compliance: frozen boundaries require freeze discipline and approvals.
- ADR discipline: architecture/governance boundary decisions require ADR entries.
- Determinism discipline: determinism-affecting changes must pass determinism gates.
- Public API boundary: changes under `include/t81/**` are compatibility-sensitive and require strict review.

Contribution guide: [CONTRIBUTING.md](CONTRIBUTING.md)

## Non-Goals

- Guaranteeing determinism outside verified surfaces.
- Guaranteeing deterministic wall-clock performance.
- Guaranteeing network timing determinism.
- Treating experimental modules as DCP-certified by default.
- Relaxing frozen ISA/data-type boundaries without formal governance process.

## Documentation Map

- **Normative Specs**: [`/spec`](spec/)
- **Architecture**: [docs/architecture/OVERVIEW.md](docs/architecture/OVERVIEW.md)
- **Governance**: [docs/governance/](docs/governance/)
- **Status**: [docs/status/](docs/status/)
- **Product**: [docs/product/](docs/product/)
- **Book (non-normative narrative)**: [book/book-en/README.md](book/book-en/README.md)

## License

MIT License. See [LICENSE](LICENSE).
