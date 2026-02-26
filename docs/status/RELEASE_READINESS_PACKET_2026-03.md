# Release Readiness Packet (2026-03)

Status: Hold (Decisioned)
Date (UTC): 2026-02-26
Owner: Release / Governance

Decision (UTC): 2026-02-26 03:10:00Z
Approver: @t81dev
Decision: HOLD

## Purpose

Consolidate release-governance evidence for the March packet cycle and preserve
GO/HOLD decision-stamp continuity.

## Scope

This packet captures governance evidence, decision fields, and required-context
verification method alignment. It does not alter CI policy, runtime semantics,
or freeze boundaries.

## Decision-Stamp Continuity

- Packet cycle 2026-02 includes explicit `Decision (UTC)`, `Approver`, and
  `Decision` fields.
- Packet cycle 2026-03 retains the same required decision-stamp fields.
- GO/HOLD basis remains tied to required branch-protection context status.

## Required-Context Verification (Procedure Alignment)

- Required contexts (branch protection):
  - `quality gate / required`
  - `Analyze (cpp)`
- Verification method:
  - `gh api repos/t81dev/t81-foundation/branches/main/protection --jq '.required_status_checks.contexts'`
  - `gh api repos/t81dev/t81-foundation/commits/<sha>/check-runs`
- Decision rule:
  - GO only when all required contexts are `completed` and `success`.
  - Otherwise HOLD.

## Decision Basis

- Current packet is issued as `HOLD` pending a release-candidate SHA with
  completed required-context verification evidence.
- No freeze model changes are introduced by this packet.
- Determinism and DCP boundaries remain unchanged.

## AGI-Facing Boundary Classification (Cycle Declaration)

- `third_party/llama.cpp` integration path (`t81_llama_adapter`, CLI
  `llama-run`) is classified as: **governed non-DCP**.
- Classification rationale:
  - AGI-facing inference/policy integration surface.
  - Outside DCP-certified deterministic core.
  - Determinism claims remain bounded to practical reproducibility evidence and
    do not imply registry-Verified deterministic guarantees.
- Promotion requirement:
  - Any upgrade beyond governed non-DCP must follow
    `docs/status/GOVERNED_AGI_PROMOTION_PIPELINE.md` and corresponding
    registry/threat-model updates.

## Candidate Verification Snapshot (2026-02-26, refreshed at 13:44:01Z)

- Candidate SHA selected: `b4fdf8efdf249e391f7c93fb18cf9245926b6a38` (`origin/main`)
- Required contexts configured on `main`:
  - `quality gate / required`
  - `Analyze (cpp)`
- Pre-close governance verification command results (local rerun):
  - `python3 scripts/governance/check_docs_governance_hygiene.py`: passed
  - `python3 scripts/governance/t81lang_promotion_gate_snapshot.py`: passed
    (`Overall result: READY`; snapshot updated at
    `docs/status/T81LANG_PROMOTION_GATE_SNAPSHOT.md`)
  - Markdown link-target sweep across `docs/status`, `docs/governance`,
    `docs/product`, and `docs/records/audits`: passed (`link-target sweep passed`)
- Required-context verification status for selected candidate:
  - `gh api repos/t81dev/t81-foundation/commits/b4fdf8efdf249e391f7c93fb18cf9245926b6a38/check-runs`
    shows `quality gate / required` is not yet emitted while upstream CI jobs
    are still in progress, and no `Analyze (cpp)` check-run is present on this
    `main` candidate snapshot.
  - `gh api repos/t81dev/t81-foundation/commits/b4fdf8efdf249e391f7c93fb18cf9245926b6a38/status`
    reports overall `state: pending`.
  - CodeQL overnight status note:
    `CodeQL` workflow runs completed successfully on PR contexts (for example,
    run `22426768501` on 2026-02-26), but this does not yet satisfy required
    `Analyze (cpp)` evidence for the selected `main` release candidate.
  - Root-cause note: `.github/workflows/codeql.yml` previously omitted `push`
    trigger for `main`, so required context `Analyze (cpp)` could not be
    produced for main-push release candidates.
- Decision implication:
  - GO criteria are not yet met for this candidate snapshot.
  - Packet remains `HOLD`.

## Open Blocking Items

1. Resolve required-context evidence mismatch for
   `b4fdf8efdf249e391f7c93fb18cf9245926b6a38`:
   required contexts are configured on branch protection but no matching
   check-run names are currently present in the candidate snapshot.
2. Land workflow trigger remediation (`.github/workflows/codeql.yml` now
   includes `push` on `main`) and re-run candidate verification on a post-fix
   main commit.
3. Capture `Analyze (cpp)` as explicit `completed` + `success` for the selected
   candidate SHA (or replace candidate SHA).
4. Re-evaluate GO/HOLD only when both required contexts are `completed` +
   `success`.

## Release Manager Checklist Pointers

- `docs/product/RELEASE_DISCIPLINE.md`
- `docs/status/RELEASE_DISCIPLINE_CHECKLIST.md`
- `.github/RELEASE_TEMPLATE.md`

## Versioning Statement

This packet is a time-bound release-governance artifact and does not override
the authority model, freeze policy, or determinism registry boundaries.
