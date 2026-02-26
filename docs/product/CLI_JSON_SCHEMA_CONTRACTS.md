# CLI JSON Schema Contracts (t81)

Status: Active
Last Updated: 2026-02-26
Owner: Product/Tooling

This document freezes machine-output JSON contracts for CLI automation.

## 1. Schema IDs

- `t81.doctor.v1` (`t81 doctor --json`)
- `t81.test.v1` (`t81 test --json`)
- `t81.fmt.v1` (`t81 fmt --json`)
- `t81.pkg-check.v1` (`t81 pkg check --json`)
- `t81.weights-info.v1` (`t81 weights info --json`)
- `t81.policy-run.v1` (`t81 policy run --json`)
- `t81.trace-export-entry.v1` (`t81 trace export --format json`)
- `t81.feedback.v1` (`t81 feedback submit ...`)
- `t81.feedback-report.v1` (`t81 feedback report`)

## 2. Compatibility Rules

1. Schema IDs are release API.
2. Existing fields in a given `*.v1` schema cannot be removed or renamed.
3. New optional fields may be added without changing the schema ID.
4. Breaking changes require a new schema ID (`*.v2`) and migration notes.

## 3. Minimum Contract Tests

CI must validate that each schema ID above is present in command output where applicable.
