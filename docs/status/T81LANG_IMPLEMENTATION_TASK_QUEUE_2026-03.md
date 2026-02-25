# T81Lang Implementation Task Queue (2026-03)

Status: Active
Owner: @t81dev
Last Updated: 2026-02-25
Scope: Execution tasks derived from A1 drift decomposition

## Purpose

Convert T81Lang drift decomposition into executable engineering tasks tied to
spec anchors, code paths, and validation tests.

## Task Queue

| Task ID | Work Item | Spec Anchor | Code Surface | Validation Target | Target Date | Status |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| A1-CODE-01 | Publish parser coverage matrix by section/production | `spec/t81lang-spec.md` section 1 + Appendix A | `lang/frontend/parser.cpp`, `lang/frontend/lexer.cpp` | `tests/cpp/frontend_parser_test.cpp`, `tests/cpp/test_parser_regression_audit.cpp`, `tests/cpp/frontend_parser_appendix_coverage_test.cpp` | 2026-03-03 | Completed (2026-02-25) |
| A1-CODE-02 | Tighten semantic/type coverage mapping to declared numeric/structural rules | `spec/t81lang-spec.md` section 2 and section 5 (Stage 3) | `lang/frontend/semantic_analyzer.cpp` | `tests/cpp/semantic_analyzer_numeric_test.cpp`, `tests/cpp/semantic_analyzer_option_result_test.cpp`, `tests/cpp/t81lang_conformance_baseline_test.cpp`, `tests/cpp/semantic_analyzer_stage3_rules_test.cpp` | 2026-03-06 | Completed (2026-02-25) |
| A1-CODE-03 | Codify deterministic compilation-profile invariants in status/spec trace docs (no runtime changes) | `spec/t81lang-spec.md` section 5 | `docs/status/VERIFIED_SURFACE_AUDIT.md`, related spec references | `tests/cpp/e2e_compile_determinism_test.cpp`, `tests/cpp/e2e_ast_ir_canonical_determinism_test.cpp`, `scripts/ci/t81lang_repro_gate.py` | 2026-03-06 | Completed (2026-02-25) |
| A1-CODE-04 | Expand reproducibility fixture coverage for currently high-drift grammar/semantic cases | `spec/t81lang-spec.md` sections 1, 2, 6 | `tests/fixtures/t81lang_determinism/` | `scripts/ci/t81lang_repro_gate.py`, `tests/cpp/e2e_compile_determinism_test.cpp`, `tests/cpp/e2e_ast_ir_canonical_determinism_test.cpp` | 2026-03-09 | Completed (2026-02-25) |
| A1-CODE-05 | Add conformance tests for uncovered match/loop/annotation edge semantics identified by matrix | `spec/t81lang-spec.md` sections 3 and 6 | frontend parser/semantic/IR paths | `tests/cpp/semantic_analyzer_match_test.cpp`, `tests/cpp/semantic_analyzer_loop_test.cpp`, `tests/cpp/e2e_match_expression_test.cpp`, `tests/cpp/t81lang_conformance_edge_semantics_test.cpp` | 2026-03-12 | Completed (2026-02-25) |
| A1-CODE-06 | Sync matrix and governance evidence after task closures | `spec/t81lang-spec.md` cross-section | status/audit artifacts | `docs/status/IMPLEMENTATION_MATRIX.md`, `docs/records/audits/2026-03-governance-review.md` | 2026-03-12 | Completed (2026-02-25) |

## Follow-On Queue (Post-A1)

| Task ID | Work Item | Spec Anchor | Code Surface | Validation Target | Target Date | Status |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| A1B-CODE-01 | Add parser coverage for expression-form `if` paths to close remaining Appendix A control-flow parser gap evidence | `spec/t81lang-spec.md` section 1 + Appendix A (`if_expression`) | `lang/frontend/parser.cpp` | `tests/cpp/frontend_parser_appendix_coverage_test.cpp`, parser-focused frontend tests | 2026-03-18 | Completed (2026-02-25) |
| A1B-CODE-02 | Publish section-level semantic coverage index for section 2/5 rule anchors using existing conformance and semantic suites | `spec/t81lang-spec.md` sections 2 and 5 | `lang/frontend/semantic_analyzer.cpp` (evidence mapping only) | `tests/cpp/semantic_analyzer_*`, `tests/cpp/t81lang_conformance_baseline_test.cpp`, `tests/cpp/semantic_analyzer_stage3_rules_test.cpp` | 2026-03-22 | Completed (2026-02-25) |
| A1B-CODE-03 | Sync matrix and governance evidence after post-A1 follow-on updates | `spec/t81lang-spec.md` cross-section | status/audit artifacts | `docs/status/IMPLEMENTATION_MATRIX.md`, `docs/records/audits/2026-03-governance-review.md`, `docs/status/T81LANG_DRIFT_DECOMPOSITION_2026-03.md` | 2026-03-25 | Completed (2026-02-25) |

## Execution Rule

Tasks in this queue must not alter freeze boundaries, determinism claim scope,
or CI policy. Any boundary-impacting change requires ADR/governance escalation.

## Cross-References

- `docs/status/T81LANG_DRIFT_DECOMPOSITION_2026-03.md`
- `docs/status/EXECUTION_PLAN_2026-03.md`
- `docs/status/IMPLEMENTATION_MATRIX.md`
- `docs/records/audits/2026-03-governance-review.md`

## Versioning Statement

This queue is an operational planning artifact and does not override `/spec` or
governance authority.
