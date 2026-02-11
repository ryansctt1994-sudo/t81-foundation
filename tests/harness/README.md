# `tests/harness`

Shared harness scripts/utilities for orchestrating deterministic test runs.

## Purpose
- Standardize local + CI execution flows.
- Reduce duplication across multi-step integration checks.

## Usage
- Run harness entry points from repository root unless noted otherwise.
- Keep output stable so CI diffs are meaningful.

## Notes
- If a harness script changes observable behavior, update dependent test docs and CI jobs.
