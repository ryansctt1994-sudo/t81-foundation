# tests/ci

Unit tests for CI policy/audit scripts under `scripts/ci/`.

## Purpose
- Verify governance and policy checks used in GitHub Actions.
- Prevent silent drift in architecture/runtime-contract/include-usage gates.

## Typical run
```bash
python3 -m unittest discover -s tests/ci -p '*_test.py'
```

## Scope
These tests validate script behavior only. They do not replace CTest/runtime verification.
