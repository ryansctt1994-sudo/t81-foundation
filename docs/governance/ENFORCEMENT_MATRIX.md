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
| **AXCHECK Deny Fail-Closed** | Yes | `build-and-test` | **Hard-Fail** | `tests/cpp/test_axion_opcodes.cpp` |
| **AXREPORT Policy Deny Fail-Closed** | Yes | `build-and-test` | **Hard-Fail** | `tests/cpp/vm_axreport_policy_deny_fail_closed_test.cpp` |
| **Policy Parse Fail-Closed (Syntax + Unknown Clause)** | Yes | `build-and-test` | **Hard-Fail** | `tests/cpp/vm_policy_parse_fail_closed_test.cpp` |
| **Public API SemVer** | No | N/A (planned) | **Warning** | Planned: API diff contract script (not yet implemented) |
| **Sandboxed Execution** | No | N/A | **Warning** | Not currently enforced as a hard CI capability |

## Policy: Multilingual Governance

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **Directory Structure Mirroring** | Yes | `spec-and-docs` | **Hard-Fail** | `scripts/governance/check_docs_structure.py` |
| **README Naming Convention** | Yes | `spec-and-docs` | **Hard-Fail** | `scripts/governance/check_readme_naming.py` |
| **Translation Metadata Headers** | Yes | `spec-and-docs` | **Hard-Fail** | `scripts/governance/check_translation_metadata.py` |
| **Staleness Threshold** | Yes | `spec-and-docs` | **Hard-Fail** | `scripts/governance/check_translation_staleness.py` (`>10` canonical commits or `>30` days behind English) |

## Policy: Dependency & Hygiene

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **No GPL/AGPL Licenses** | Yes | `spec-and-docs` | **Hard-Fail** | `scripts/governance/check_license_policy.py` |
| **Workflow Action Pinning** | Yes | `spec-and-docs`, `quality-gate` | **Hard-Fail** | `scripts/ci/audit_workflow_actions.py` |
| **Workflow Permissions Policy** | Yes | `spec-and-docs`, `quality-gate` | **Hard-Fail** | `scripts/ci/audit_workflow_permissions.py` |
| **Public Header Root (`include/t81/**`)** | Yes | `deterministic-core-profile-check` | **Hard-Fail** | Inline CI check in `.github/workflows/ci.yml` |
| **Artifact Containment (repo cleanliness)** | Yes | `spec-and-docs` | **Hard-Fail** | `scripts/governance/check_repo_artifact_hygiene.py` |

## Policy: System Status & Implementation

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **Spec-Code Alignment Coverage** | Partial | `spec-and-docs` + governance audits | **Warning** | `scripts/governance/check_docs_governance_hygiene.py` + status artifacts (no single full spec-coverage script yet) |

## Policy: Project Control Center

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **Root Directory Freeze** | Yes | `spec-and-docs` | **Hard-Fail** | `scripts/governance/check_root_structure.py` |
| **Benchmark Performance Gating** | Partial | `benchmarks` (non-blocking), optional benchmark workflows | **Warning** | `scripts/ci/check_simd_regression.py` exists, but not currently wired as a required hard gate |
