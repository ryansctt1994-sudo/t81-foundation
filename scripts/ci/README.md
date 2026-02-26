# scripts/ci

CI policy and reproducibility gate scripts used by `.github/workflows/ci.yml`.

## Categories
- Reproducibility gates: `t81lang_repro_gate.py`, `t3k_repro_gate.py`, `llama_cpp_repro_gate.py`
- Determinism slice runner: `run_determinism_slice.sh`
- Governance/audit checks: workflow action pinning, permissions, architecture sync
- Numeric policy checks: legacy include/type usage and wrapper-thinness guards

## Local invocation examples
```bash
python3 scripts/ci/check_architecture_targets.py
python3 scripts/ci/t81lang_repro_gate.py --help
python3 scripts/ci/t3k_repro_gate.py --help
python3 scripts/ci/llama_cpp_repro_gate.py --help
bash scripts/ci/run_determinism_slice.sh build
```
