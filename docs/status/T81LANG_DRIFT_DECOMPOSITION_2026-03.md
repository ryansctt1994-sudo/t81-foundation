# T81Lang Drift Decomposition (2026-03)

Status: Active
Owner: @t81dev
Last Updated: 2026-02-25
Target Completion: 2026-03-12 (A1 planning milestone)

## Purpose

Decompose T81Lang high-drift status into concrete milestones with measurable
status targets and governance evidence paths.

## Scope

This decomposition addresses status drift between:

- `spec/t81lang-spec.md`
- implementation surfaces under `lang/frontend/` and `lang/stdlib/`
- determinism/reproducibility evidence in CI and fixture gates

It does not redefine T81Lang semantics, freeze boundaries, or DCP scope.

## Baseline Signals

- `docs/status/IMPLEMENTATION_MATRIX.md`: T81Lang marked Experimental / High drift.
- `docs/status/VERIFIED_SURFACE_AUDIT.md`: compiler bytecode emission listed as
  Partial Traceability with a spec-gap note for deterministic compilation
  profile details.
- `scripts/ci/t81lang_repro_gate.py` and
  `tests/fixtures/t81lang_determinism/`: active reproducibility evidence path.

## Milestone Checklist

### M1 — Drift Surface Inventory Lock

- Target Date: 2026-03-03
- Objective:
  - Freeze a documented inventory of T81Lang spec sections vs implementation
    coverage categories (implemented / partial / missing / experimental).
- Acceptance Criteria:
  - Inventory table committed in this document.
  - Table references concrete code or test evidence for each entry.
- Evidence:
  - This file
  - `docs/status/IMPLEMENTATION_MATRIX.md`

### M2 — Deterministic Compilation Profile Gap Plan

- Target Date: 2026-03-06
- Objective:
  - Convert current verified-surface audit gap statement into a bounded action
    plan for deterministic compilation-profile documentation.
- Acceptance Criteria:
  - Planned spec-update scope documented (no semantic expansion in this step).
  - Evidence path for future implementation/test linkage identified.
- Evidence:
  - `docs/status/VERIFIED_SURFACE_AUDIT.md`
  - `spec/t81lang-spec.md` (planned section target only)

### M3 — Conformance and Repro Evidence Mapping

- Target Date: 2026-03-09
- Objective:
  - Map existing T81Lang conformance and reproducibility checks to drift
    categories so status can be objectively updated.
- Acceptance Criteria:
  - Explicit mapping of fixture gate, conformance test(s), and frontend tests to
    decomposition categories.
  - Coverage gaps captured as follow-up items, not claims.
- Evidence:
  - `scripts/ci/t81lang_repro_gate.py`
  - `tests/fixtures/t81lang_determinism/`
  - `tests/cpp/t81lang_conformance_baseline_test.cpp`
  - `tests/cpp/frontend_*`

### M4 — Matrix and Governance Sync Update

- Target Date: 2026-03-12
- Objective:
  - Update status artifacts to reflect decomposed milestones and current
    progress state.
- Acceptance Criteria:
  - `IMPLEMENTATION_MATRIX.md` T81Lang notes reference this decomposition.
  - March governance review note includes A1 progress snapshot.
- Evidence:
  - `docs/status/IMPLEMENTATION_MATRIX.md`
  - `docs/records/audits/2026-03-governance-review.md`

## Decomposition Table (Initial)

| Area | Spec Anchor | Current Signal | Status Category | Evidence |
| :--- | :--- | :--- | :--- | :--- |
| Grammar and syntax definition | `spec/t81lang-spec.md` section 1 | Spec present; implementation parity not fully declared | Partial | Frontend parser/lexer tests |
| Type-system surface | `spec/t81lang-spec.md` section 2 | Draft scope exceeds currently asserted status guarantees | Partial | Semantic analyzer test families |
| Deterministic bytecode emission profile | Gap noted in verified audit | Explicit audit gap | Missing (doc specificity gap) | `docs/status/VERIFIED_SURFACE_AUDIT.md` |
| Reproducibility fixture gate | Compiler repro row in registry | Active fixture hash gate | Implemented evidence path (partial registry status) | `scripts/ci/t81lang_repro_gate.py` |
| Conformance baseline | T81Lang conformance test | Active baseline test exists | Partial | `tests/cpp/t81lang_conformance_baseline_test.cpp` |

## Risks and Controls

- Risk: Overclaiming deterministic guarantees for T81Lang surfaces.
  - Control: Keep registry status language authoritative; avoid guarantee
    expansion in decomposition updates.
- Risk: Planning drift without evidence updates.
  - Control: Each milestone requires artifact-level evidence.

## Cross-References

- `docs/status/EXECUTION_PLAN_2026-03.md`
- `docs/status/IMPLEMENTATION_MATRIX.md`
- `docs/status/VERIFIED_SURFACE_AUDIT.md`
- `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`
- `docs/governance/SPEC_AUTHORITY_MODEL.md`

## Versioning Statement

This decomposition is a planning-control artifact. It does not alter normative
specification authority or deterministic guarantee boundaries.
