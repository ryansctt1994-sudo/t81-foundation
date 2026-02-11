# scripts

Operational scripts for CI gates, demos, and maintenance workflows.

## Structure
- `ci/`: reproducibility gates, workflow audits, policy checks
- `notebooks/`: notebook helper assets
- top-level utilities: runtime-contract sync, TOC/sidebar generation, demo runners

## High-use scripts
- `scripts/ci/t81lang_repro_gate.py`
- `scripts/ci/t3k_repro_gate.py`
- `scripts/ci/run_determinism_slice.sh`
- `scripts/check-runtime-contract-sync.py`
- `scripts/run-canonical-runtime-demo.sh`

## Usage notes
- Keep script output deterministic where used in gates.
- Prefer explicit exit codes and concise failure messages.
- Update `docs/ci.md` when gate behavior changes.
