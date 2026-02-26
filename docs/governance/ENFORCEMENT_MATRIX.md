# Governance Enforcement Matrix

This document translates high-level governance policies into machine-verifiable enforcement rules.

## Enforcement Definitions
*   **Hard-Fail**: CI pipeline fails immediately; merge blocked.
*   **Soft-Fail**: CI warns; requires manual override or specific label to merge.
*   **Warning**: Non-blocking; flagged in report for future remediation.

## Policy: Capability Contract

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **Bit-Exact Determinism** | Yes | `build-and-test`, `determinism-slice`, `t81lang-cross-arch-bit-identity`, `t3k-cross-arch-bit-identity`, `quality-gate` | **Hard-Fail** | `scripts/ci/t81lang_repro_gate.py`, `scripts/ci/t3k_repro_gate.py`, `scripts/ci/run_determinism_slice.sh` |
| **ISA Stability (Frozen)** | Yes | `spec-and-docs`, `quality-gate` | **Hard-Fail** | `scripts/ci/check_tisc_freeze_integrity.py` |
| **Unimplemented Privileged Opcode Fail-Closed** | Yes | `build-and-test` | **Hard-Fail** | `tests/cpp/vm_stubbed_privileged_opcode_fail_closed_test.cpp` |
| **Unimplemented Async/Network Opcode Fail-Closed** | Yes | `build-and-test` | **Hard-Fail** | `tests/cpp/vm_stubbed_async_network_opcode_fail_closed_test.cpp` |
| **Unimplemented Neural Opcode Fail-Closed** | Yes | `build-and-test` | **Hard-Fail** | `tests/cpp/test_vm_neural_opcodes.cpp` |
| **Public API SemVer** | No | N/A (planned) | **Warning** | Planned: API diff contract script (not yet implemented) |
| **Sandboxed Execution** | No | N/A | **Warning** | Not currently enforced as a hard CI capability |

## Policy: Multilingual Governance

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **Directory Structure Mirroring** | No | N/A (planned) | **Warning** | Planned: `check_docs_structure.py` (not yet implemented) |
| **README Naming Convention** | No | N/A (planned) | **Warning** | Planned: naming policy checker (not yet implemented) |
| **Translation Metadata Headers** | No | N/A (planned) | **Warning** | Planned: translation metadata checker (not yet implemented) |
| **Staleness Threshold** | No | N/A (planned) | **Warning** | Planned: translation staleness checker (not yet implemented) |

## Policy: Dependency & Hygiene

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **No GPL/AGPL Licenses** | No | N/A (planned) | **Warning** | Planned: license policy checker (not yet implemented) |
| **Workflow Action Pinning** | Yes | `spec-and-docs`, `quality-gate` | **Hard-Fail** | `scripts/ci/audit_workflow_actions.py` |
| **Workflow Permissions Policy** | Yes | `spec-and-docs`, `quality-gate` | **Hard-Fail** | `scripts/ci/audit_workflow_permissions.py` |
| **Public Header Root (`include/t81/**`)** | Yes | `deterministic-core-profile-check` | **Hard-Fail** | Inline CI check in `.github/workflows/ci.yml` |
| **Artifact Containment (repo cleanliness)** | No | N/A (planned) | **Warning** | Planned: git-clean checker (not yet implemented) |

## Policy: System Status & Implementation

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **Spec-Code Alignment Coverage** | Partial | `spec-and-docs` + governance audits | **Warning** | `scripts/governance/check_docs_governance_hygiene.py` + status artifacts (no single full spec-coverage script yet) |

## Policy: Project Control Center

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **Root Directory Freeze** | No | N/A (planned) | **Warning** | Planned: root-structure checker (not yet implemented) |
| **Benchmark Performance Gating** | Partial | `benchmarks` (non-blocking), optional benchmark workflows | **Warning** | `scripts/ci/check_simd_regression.py` exists, but not currently wired as a required hard gate |
