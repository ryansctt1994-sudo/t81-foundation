# ADR-0002: Deterministic Core Profile Product Boundary

Status: Accepted
Date: 2026-02-25
Version: 1.0.0

## Purpose

Establish DCP as the release-claim boundary for deterministic guarantees.

## Scope

Applies to product claims, release gating, and freeze-discipline interpretation
for DCP-covered surfaces.

## Context

The repository contains stable and experimental surfaces. Governance requires a
clear release boundary to prevent overclaim drift.

## Decision

Use `docs/product/DETERMINISTIC_CORE_PROFILE.md` as the canonical product
boundary for deterministic guarantees and release claims. Releases claiming DCP
conformance must satisfy release discipline and verified-surface evidence.

## Alternatives Considered

- Single global guarantee for all repository surfaces: rejected due to
  experimental and partial implementations.
- No formal product boundary: rejected due to ambiguity in release guarantees.

## Consequences

- Separates stable guarantees from experimental work.
- Requires explicit promotion path for non-DCP surfaces.
- Tightens release readiness requirements for DCP claims.

## References

- `docs/product/DETERMINISTIC_CORE_PROFILE.md`
- `docs/governance/FREEZE_ENFORCEMENT.md`
- `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`
- `docs/product/RELEASE_DISCIPLINE.md`

## Versioning Statement

DCP boundary changes that alter guarantees require ADR updates and corresponding
governance/versioning actions under freeze enforcement.
