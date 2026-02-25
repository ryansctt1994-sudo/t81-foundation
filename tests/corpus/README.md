# Deterministic Regression Corpus

Status: Active
Version: 1.0.0
Owner: Tests/Governance
Last Updated: 2026-02-25

## Purpose

Define a stable long-term determinism corpus that anchors reproducibility checks
to canonical fixture programs and hash artifacts.

## Scope

This corpus references existing deterministic fixtures and hash ledgers used by
current reproducibility gates. It does not introduce new language/runtime
features.

## Canonical Programs

Primary canonical fixture set:

- `tests/fixtures/t81lang_determinism/01_bigint_add.t81`
- `tests/fixtures/t81lang_determinism/02_fraction_sub.t81`
- `tests/fixtures/t81lang_determinism/03_float_literal.t81`
- `tests/fixtures/t81lang_determinism/04_mixed_widen_float.t81`
- `tests/fixtures/t81lang_determinism/05_bool_and_string.t81`
- `tests/fixtures/t81lang_determinism/06_mixed_widen_fraction.t81`
- `tests/fixtures/t81lang_determinism/07_if_else_print.t81`
- `tests/fixtures/t81lang_determinism/08_bounded_loop_print.t81`
- `tests/fixtures/t81lang_determinism/09_nested_if_print.t81`
- `tests/fixtures/t81lang_determinism/10_relation_bool_print.t81`
- `tests/fixtures/t81lang_determinism/11_match_option_some_print.t81`
- `tests/fixtures/t81lang_determinism/12_match_option_guard_print.t81`
- `tests/fixtures/t81lang_determinism/13_match_enum_payload_guard_print.t81`
- `tests/fixtures/t81lang_determinism/14_result_match_guard_print.t81`
- `tests/fixtures/t81lang_determinism/15_bitwise_shift_chain_print.t81`
- `tests/fixtures/t81lang_determinism/16_symbol_equality_branch_print.t81`

## Hash Expectations Format

Hash references are maintained in:

- `tests/fixtures/t81lang_determinism/t81lang_repro_hash.txt`
- `tests/fixtures/t81lang_determinism/t81lang_ast_ir_repro_hash.txt`

Format requirement:

- one deterministic digest record per artifact class
- generated from reproducibility gate outputs
- reviewed as part of determinism governance updates

## Cross-Architecture Verification Procedure

1. Run determinism gate script:
   - `python3 scripts/ci/t81lang_repro_gate.py`
2. Compare generated hash outputs with committed hash references.
3. Validate parity on supported architectures listed in project policy.
4. If mismatch occurs on a verified surface, handle under:
   - `docs/governance/INCIDENT_RESPONSE.md`

## Cross-References

- `docs/governance/DETERMINISM_SURFACE_REGISTRY.md`
- `docs/governance/DETERMINISM_THREAT_MODEL.md`
- `docs/product/DETERMINISTIC_CORE_PROFILE.md`
- `docs/status/DETERMINISTIC_CORPUS_MANIFEST.md`

## Versioning Statement

Corpus membership and hash reference changes must be version-controlled and
reviewed under determinism governance constraints.
