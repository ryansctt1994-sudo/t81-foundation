# Release Discipline Checklist

**Status:** Active Protocol
**Authority:** `docs/governance/FREEZE_ENFORCEMENT.md`

This document defines the rigorous release discipline required to maintain the "Institutional Grade" status of the T81 Foundation stack.

## 1. Versioning Triggers

We strictly follow Semantic Versioning (SemVer 2.0.0).

| Change Type | Version Bump | Rationale |
| :--- | :---: | :--- |
| **ISA Semantic Change** | **MAJOR** | Invalidates existing bytecode execution traces. |
| **Data Type Binary Format** | **MAJOR** | Invalidates existing serialized data. |
| **Determinism Regression** | **MAJOR** | Breaks the verification contract. |
| **New Opcode (Compatible)** | **MINOR** | Backward-compatible ISA extension. |
| **New Public API** | **MINOR** | Backward-compatible API addition. |
| **Performance Optimization** | **PATCH** | No observable behavior change. |
| **Bug Fix (Non-Spec)** | **PATCH** | Fixes divergence from spec. |

## 2. Release Artifacts

Every release MUST generate and archive the following artifacts:

1.  **Source Tarball**: Deterministic `tar.gz` of the release commit.
2.  **Binary Signatures**: GPG signatures for all binaries.
3.  **Reproducibility Report**: Output of `scripts/ci/repro-ledger.py` proving bit-exactness on reference platforms.
4.  **Spec Drift Report**: Output of `scripts/governance/spec_impl_drift_check.py`.
5.  **Verified Surface Audit**: Updated `docs/status/VERIFIED_SURFACE_AUDIT.md`.

## 3. Supported Platforms

The root `README.md` MUST explicitly declare the "Supported Platforms" for the release.
**Determinism is ONLY guaranteed on these platforms.**

Current Supported Platforms (v1.x):
*   **x86-64 Linux** (Ubuntu 22.04+)
*   **ARM64 macOS** (Apple Silicon)

## 4. Pre-Release Audit Checklist

Before tagging any release (vX.Y.Z), the Release Manager MUST verify:

- [ ] **CI Green**: All workflows in `.github/workflows/` are passing.
- [ ] **Drift Check**: `scripts/governance/spec_impl_drift_check.py` returns success (or known waivers).
- [ ] **Determinism Gate**: `repro-ledger` confirms bit-exact output on both x86-64 and ARM64.
- [ ] **Spec Frozen**: No pending PRs modifying `spec/` for frozen components.
- [ ] **Changelog**: `CHANGELOG.md` updated with "BREAKING CHANGES" section if applicable.
- [ ] **Migration Guide**: If MAJOR bump, `docs/migration/vX_to_vY.md` exists.
- [ ] **Audit Update**: `docs/status/VERIFIED_SURFACE_AUDIT.md` reflects current status.

## 5. Emergency Patch Protocol

For Critical Security or Determinism Fixes:

1.  **Branch**: Create `release/vX.Y.Z-hotfix`.
2.  **Minimal Fix**: Apply ONLY the necessary fix. No refactoring.
3.  **Regression Test**: Add a test case reproducing the failure.
4.  **Verify**: Ensure no *other* determinism surfaces regressed.
5.  **Tag**: Release as PATCH version.
