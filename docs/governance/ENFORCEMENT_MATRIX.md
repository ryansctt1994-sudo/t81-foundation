# Governance Enforcement Matrix

This document translates high-level governance policies into machine-verifiable enforcement rules.

## Enforcement Definitions
*   **Hard-Fail**: CI pipeline fails immediately; merge blocked.
*   **Soft-Fail**: CI warns; requires manual override or specific label to merge.
*   **Warning**: Non-blocking; flagged in report for future remediation.

## Policy: Capability Contract

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **Bit-Exact Determinism** | Yes | `determinism-check` | **Hard-Fail** | `scripts/ci/t81lang_repro_gate.py` |
| **ISA Stability (Frozen)** | Yes | `isa-compatibility-check` | **Hard-Fail** | `scripts/ci/check_opcode_compat.py` |
| **Public API SemVer** | Partial | `api-diff-check` | **Soft-Fail** | `scripts/ci/api_diff.sh` |
| **Sandboxed Execution** | Yes | `security-sandbox-test` | **Hard-Fail** | `tests/security/sandbox_tests` |

## Policy: Multilingual Governance

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **Directory Structure Mirroring** | Yes | `docs-structure-check` | **Soft-Fail** | `scripts/ci/check_docs_structure.py` |
| **README Naming Convention** | Yes | `docs-structure-check` | **Warning** | `scripts/ci/check_docs_structure.py` |
| **Translation Metadata Headers** | Yes | `translation-metadata-check` | **Warning** | `scripts/ci/check_translation_metadata.py` |
| **Staleness Threshold** | Yes | `translation-staleness-check` | **Warning** | `scripts/ci/check_translation_staleness.py` |

## Policy: Dependency & Hygiene

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **No GPL/AGPL Licenses** | Yes | `license-check` | **Hard-Fail** | `scripts/ci/check_licenses.py` |
| **C++ Dependency Pinning** | Yes | `cmake-audit` | **Hard-Fail** | `scripts/ci/audit_cmake_deps.py` |
| **No Header Vendoring** | Yes | `include-audit` | **Soft-Fail** | `scripts/ci/audit_include_dir.py` |
| **No Runtime JS Dependencies** | Yes | `js-dependency-check` | **Hard-Fail** | `scripts/ci/check_package_json.py` |
| **Artifact Containment** | Yes | `git-clean-check` | **Hard-Fail** | `scripts/ci/check_git_clean.sh` |

## Policy: System Status & Implementation

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **Spec-Code Alignment Coverage** | Partial | `spec-coverage-report` | **Warning** | `scripts/ci/spec_coverage.py` |

## Policy: Project Control Center

| Rule | Machine-Verifiable | Required CI Job | Enforcement Severity | Required Scripts / Metadata |
| :--- | :---: | :--- | :--- | :--- |
| **Root Directory Freeze** | Yes | `root-dir-check` | **Hard-Fail** | `scripts/ci/check_root_structure.py` |
| **Benchmark Performance Gating** | Yes | `benchmark-gate` | **Soft-Fail** | `scripts/ci/check_simd_regression.py` |
