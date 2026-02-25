# Release Discipline Manifest

Status: Active
Version: 1.0.0
Owner: Product/Governance
Last Updated: 2026-02-25

## Purpose

Define release preconditions, versioning discipline, and release evidence for
tagged versions `vX.Y.Z`.

## Scope

Applies to all releases claiming Deterministic Core Profile (DCP) guarantees and
freeze-boundary compliance.

## Definitions

- DCP: Deterministic Core Profile defined in
  `docs/product/DETERMINISTIC_CORE_PROFILE.md`.
- Freeze exception: Explicit, reviewed boundary exception per governance policy.

## 1. Release Preconditions

Before tagging `vX.Y.Z`, all of the following must hold:

1. Determinism registry verified surfaces are current and accurate.
2. DCP compliance is confirmed for release scope.
3. Structural integrity scripts and required checks pass.
4. No open freeze exceptions.
5. No open determinism breach on verified surfaces.

## 2. Versioning Rules

This manifest is interpreted with:

- `docs/governance/FREEZE_ENFORCEMENT.md`
- `docs/product/DETERMINISTIC_CORE_PROFILE.md`

SemVer mapping:

- MAJOR: Frozen boundary violation or backward-incompatible change.
- MINOR: Backward-compatible feature addition.
- PATCH: Bug fix only; no compatibility or deterministic-contract expansion.

## 3. Release Checklist

- [ ] Registry status reviewed: `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`
- [ ] DCP scope validated: `docs/product/DETERMINISTIC_CORE_PROFILE.md`
- [ ] Structural integrity status validated:
      `docs/status/STRUCTURAL_INTEGRITY_REPORT.md`
- [ ] Verified surface audit current:
      `docs/status/VERIFIED_SURFACE_AUDIT.md`
- [ ] No open freeze exception labels for target release scope
- [ ] No open determinism incident at Severity 2 or Severity 3
- [ ] Release notes include determinism hash summary and experimental delta summary

## Cross-References

- `docs/governance/SPEC_AUTHORITY_MODEL.md`
- `docs/governance/FREEZE_ENFORCEMENT.md`
- `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`
- `docs/governance/DETERMINISM_THREAT_MODEL.md`
- `docs/governance/INCIDENT_RESPONSE.md`
- `docs/product/DETERMINISTIC_CORE_PROFILE.md`

## Versioning Statement

This manifest is governance policy. Any relaxation of release preconditions
requires explicit governance review and ADR traceability.
