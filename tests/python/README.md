# `tests/python`

Python-based tests and utilities for repo validation.

## Scope
- Tooling checks where Python is the most practical implementation.
- Cross-checks complementing C++ test coverage.

## Running
- Use project test commands documented in root `README.md` and `tests/README.md`.
- Keep scripts hermetic and deterministic (no network side effects).

## Notes
- Prefer explicit inputs/outputs in tests to keep failures diagnosable.
