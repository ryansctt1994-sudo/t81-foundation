# Axion Partial-Coverage Alignment Plan (2026-03)

Status: Active
Owner: @t81dev
Last Updated: 2026-02-25
Target Completion: 2026-03-19 (A2 planning milestone)

## Purpose

Define a bounded alignment plan for Axion Kernel partial coverage against draft
spec scope, with dated milestones and evidence paths.

## Scope

This plan addresses status/control alignment between:

- `spec/axion-kernel.md`
- implementation surfaces under `kernel/axion/`
- status/governance evidence artifacts

This plan does not change normative semantics, freeze boundaries, determinism
scope, or DCP guarantees.

## Baseline Gap Statement

- `docs/status/IMPLEMENTATION_MATRIX.md` marks Axion Kernel as Partial / Medium
  drift.
- `docs/status/SYSTEM_STATUS.md` marks Axion Kernel as Alpha with partial
  implementation against draft surfaces.
- Current status notes describe gap posture but not milestone-bounded closure
  actions.

## Milestone Checklist

### M1 — Draft Scope Segmentation

- Target Date: 2026-03-05
- Objective:
  - Segment `spec/axion-kernel.md` scope into implementation buckets:
    implemented, partial, missing, deferred.
- Acceptance Criteria:
  - Segmentation table published in this document.
  - Each segment references implementation or test surface where available.
- Evidence:
  - This file
  - `docs/status/IMPLEMENTATION_MATRIX.md`

### M2 — Coverage Gap Prioritization

- Target Date: 2026-03-10
- Objective:
  - Prioritize partial/missing segments into a bounded queue with owner and
    target dates.
- Acceptance Criteria:
  - Top-priority gaps recorded with rationale and constraints.
  - Deferred items explicitly marked as non-DCP and non-verified unless
    promoted through governance.
- Evidence:
  - This file
  - `docs/status/SYSTEM_STATUS.md`

### M3 — Evidence Mapping

- Target Date: 2026-03-14
- Objective:
  - Map current tests/checks to prioritized Axion segments.
- Acceptance Criteria:
  - For each priority segment, evidence path is declared (test, integration
    surface, or pending evidence).
  - Uncovered segments are explicitly listed as open.
- Evidence:
  - This file
  - `tests/` (relevant Axion-related test surfaces)

### M4 — Matrix and Review Synchronization

- Target Date: 2026-03-19
- Objective:
  - Synchronize Axion plan status into matrix and monthly governance review.
- Acceptance Criteria:
  - Matrix notes reference this alignment plan.
  - March governance review records A2 planning completion state.
- Evidence:
  - `docs/status/IMPLEMENTATION_MATRIX.md`
  - `docs/records/audits/2026-03-governance-review.md`

## Bounded Next Actions

| Action ID | Action | Owner | Target Date | Status |
| :--- | :--- | :--- | :--- | :--- |
| A2-N1 | Publish segmented scope table for draft Axion surfaces | @t81dev | 2026-03-05 | Planned |
| A2-N2 | Prioritize top 3 partial/missing segments with dated targets | @t81dev | 2026-03-10 | Planned |
| A2-N3 | Attach evidence paths for each prioritized segment | @t81dev | 2026-03-14 | Planned |
| A2-N4 | Sync matrix and governance review with A2 snapshot | @t81dev | 2026-03-19 | Planned |

## Cross-References

- `docs/status/EXECUTION_PLAN_2026-03.md`
- `docs/status/IMPLEMENTATION_MATRIX.md`
- `docs/status/SYSTEM_STATUS.md`
- `docs/governance/SPEC_AUTHORITY_MODEL.md`
- `docs/product/DETERMINISTIC_CORE_PROFILE.md`

## Versioning Statement

This artifact is a status/control plan and does not override `/spec` or freeze
policy.
