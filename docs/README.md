# Documentation Index

> **Source of Truth:** This document maps the documentation hierarchy for T81 Foundation.

**Last Updated:** February 10, 2026

## Governance & Policy

- **[GOVERNANCE.md](GOVERNANCE.md):** Project governance model and decision-making process.
- **[MAINTAINERS.md](MAINTAINERS.md):** List of current maintainers and roles.
- **[SUPPORT.md](SUPPORT.md):** Support channels and contact information.
- **[RISKS.md](RISKS.md):** Known risks and mitigation strategies.
- **[VERSIONING.md](../VERSIONING.md):** Release and compatibility policy.

## Technical Guides

- **[Research Guide](research-guide.md):** Deep dive into the T81 research goals.
- **[AI Quickstart](ai-quickstart.md):** Using T81 with AI models.
- **[System Integration](system-integration.md):** Integrating T81 into larger systems.
- **[CI/CD](ci.md):** Continuous Integration and deployment.
- **[Runtime Boundary](runtime-semantics-boundary.md):** Defining the boundary between spec and implementation.
- **[EVIDENCE.md](EVIDENCE.md):** Proof of claims matrix.
- **[REPRODUCIBILITY.md](REPRODUCIBILITY.md):** Instructions for reproducible builds.
- **[TESTING.md](TESTING.md):** Testing strategy and taxonomy.

## Reference

- **[Audits](audits/):** Repository audits.
- **[Benchmarks](benchmarks.md):** Performance benchmarks.

## About this directory

This directory contains the core documentation for T81 Foundation architecture, operations, and user/developer guides.
- **Core references:** `ci.md`, `system-status.md`, `release.md`, `navigation.md`
- **Guides:** `guides/` (CLI, Axion, weights, runtime observability, feature workflows)
- **Research and design:** `research-guide.md`, `jit-research.md`, `hardware-roadmap.md`
- **Audits and governance artifacts:** `audits/`, `proposals/`, `archive/`
- **Site/Jekyll scaffolding:** `_layouts/`, `_includes/`, `assets/`, `index.md`

**Validation:**
Useful checks before merging doc-heavy changes:
```bash
python3 scripts/ci/check_architecture_targets.py
python3 scripts/check-runtime-contract-sync.py
```
