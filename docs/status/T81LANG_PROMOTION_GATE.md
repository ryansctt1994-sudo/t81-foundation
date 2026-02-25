# T81Lang Promotion Gate (Draft -> Beta)

Status: Active
Owner: @t81dev
Last Updated: 2026-02-25
Current Classification: Draft Spec / Experimental Implementation
Target Transition: Draft -> Beta (candidate)

## Purpose

Define an explicit, auditable promotion gate for moving T81Lang from Draft /
Experimental posture toward Beta-candidate review.

## Scope

This gate applies to T81Lang specification/implementation alignment evidence in:

- `spec/t81lang-spec.md`
- `docs/status/T81LANG_DRIFT_DECOMPOSITION_2026-03.md`
- `docs/status/T81LANG_IMPLEMENTATION_TASK_QUEUE_2026-03.md`
- `docs/status/IMPLEMENTATION_MATRIX.md`
- `docs/status/VERIFIED_SURFACE_AUDIT.md`

This artifact does not modify runtime semantics, freeze boundaries, or
determinism claim scope.

## Definitions

- Gate criterion: A required pass/fail condition for promotion readiness.
- Beta-candidate review: Governance decision point where all gate criteria are
  either passed or explicitly blocked with owner/due-date remediation.
- Verified-surface boundary: Determinism guarantee scope defined by
  `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`.

## Gate Criteria

| ID | Criterion | Evidence Source | Required State |
| :--- | :--- | :--- | :--- |
| TG-01 | Spec-to-implementation drift decomposition has active closure tracking and no stale completed/planned conflicts | `docs/status/T81LANG_DRIFT_DECOMPOSITION_2026-03.md`, `docs/status/T81LANG_IMPLEMENTATION_TASK_QUEUE_2026-03.md` | Pass |
| TG-02 | Deterministic compile/repro evidence remains green across canonical fixture pack | `scripts/ci/t81lang_repro_gate.py`, `tests/fixtures/t81lang_determinism/`, `tests/cpp/e2e_compile_determinism_test.cpp` | Pass |
| TG-03 | Core semantic conformance for sections 2/3/4/5/6 remains green in mapped suites | `tests/cpp/t81lang_conformance_baseline_test.cpp`, `tests/cpp/semantic_analyzer_*`, `tests/cpp/e2e_*` | Pass |
| TG-04 | Open gaps are reduced to a bounded, owner-assigned engineering backlog with acceptance tests | `docs/status/T81LANG_ENGINEERING_BACKLOG_2026-03.md` | Pass |
| TG-05 | Matrix and governance artifacts explicitly reflect current promotion posture | `docs/status/IMPLEMENTATION_MATRIX.md`, `docs/records/audits/2026-03-governance-review.md` | Pass |
| TG-06 | Determinism claims remain bounded to verified registry surfaces (no overclaim) | `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`, `docs/product/DETERMINISTIC_CORE_PROFILE.md` | Pass |

## Current Gate Snapshot (2026-02-25)

| Criterion | Status | Notes |
| :--- | :--- | :--- |
| TG-01 | Pass | A1 through A1G queues and synchronization entries are closed and cross-linked. |
| TG-02 | Pass | Repro gate and compile determinism checks are passing in current cycle snapshots. |
| TG-03 | Pass | Current mapped semantic/conformance suites are green for tracked surfaces. |
| TG-04 | In Progress | Ranked engineering backlog is defined; implementation sprint execution is pending. |
| TG-05 | Pass | Matrix + audit artifacts are synchronized with current closure evidence. |
| TG-06 | Pass | Registry-bounded determinism language remains intact in status/governance artifacts. |

Promotion readiness decision:

- Result: Not Ready (as of 2026-02-25)
- Blocking criterion(s): TG-04

## Blocking Items (for Beta-candidate review)

1. Complete prioritized engineering backlog items in
   `docs/status/T81LANG_ENGINEERING_BACKLOG_2026-03.md`.
2. Re-run gate snapshot and set result to either:
   - Ready for Beta-candidate review, or
   - Not Ready with updated blockers.

## Cross-References

- `docs/status/T81LANG_DRIFT_DECOMPOSITION_2026-03.md`
- `docs/status/T81LANG_IMPLEMENTATION_TASK_QUEUE_2026-03.md`
- `docs/status/T81LANG_ENGINEERING_BACKLOG_2026-03.md`
- `docs/status/IMPLEMENTATION_MATRIX.md`
- `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`
- `docs/product/DETERMINISTIC_CORE_PROFILE.md`

## Versioning Statement

This gate is a governance/status control artifact. It does not override `/spec`
or freeze policy.
