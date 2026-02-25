# Release Discipline Checklist (Status View)

Status: Active
Last Updated: 2026-02-25
Owner: Release / Governance

## Purpose

Provide the operational execution checklist for release managers.

## Source of Truth

Normative release policy and SemVer interpretation are defined in:

- `docs/product/RELEASE_DISCIPLINE.md`
- `docs/governance/FREEZE_ENFORCEMENT.md`
- `docs/product/DETERMINISTIC_CORE_PROFILE.md`

This status checklist must remain aligned with those documents and must not
duplicate or redefine their versioning policy.

## Pre-Tag Checklist (`vX.Y.Z`)

- [ ] CI required checks green for target commit
- [ ] Determinism registry status reviewed:
      `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`
- [ ] DCP scope validated:
      `docs/product/DETERMINISTIC_CORE_PROFILE.md`
- [ ] Structural integrity status reviewed:
      `docs/status/STRUCTURAL_INTEGRITY_REPORT.md`
- [ ] Verified surface audit current:
      `docs/status/VERIFIED_SURFACE_AUDIT.md`
- [ ] Deterministic corpus references current:
      `docs/status/DETERMINISTIC_CORPUS_MANIFEST.md`
- [ ] No open freeze exceptions for release scope
- [ ] No open Severity 2/3 determinism incidents
- [ ] Release notes include determinism hash summary and experimental delta

## Required Release Artifacts

1. Source package for tagged commit.
2. Determinism evidence summary (gate outputs and hash references).
3. Structural integrity status reference.
4. Verified surface audit reference.
5. Changelog/release notes with breaking-change visibility where applicable.

## Cross-References

- `docs/status/RELEASE_READINESS_PACKET_2026-02.md`
- `.github/RELEASE_TEMPLATE.md`

## Versioning Statement

Checklist changes must not weaken release controls defined in the source policy
documents.
