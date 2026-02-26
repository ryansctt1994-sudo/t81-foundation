# llama.cpp reproducibility fixture pack

This fixture pack defines the sanctioned inputs for the optional
`llama_cpp_repro_gate.py` CI path.

## Files in this directory

- `policy.apl`: Axion policy used for `t81 llama-run`
- `prompt.txt`: deterministic prompt text used by the gate
- `model.gguf`: required model file (not checked in)
- `model_hash.txt`: required expected model hash (not checked in)
- `model_hash.txt.example`: template for expected model hash format

## Model handling policy

A real `model.gguf` is intentionally not committed in this repository due to
size and licensing constraints.

To run the gate locally or in CI:

1. Place the sanctioned model at:
   - `tests/fixtures/llama_cpp_repro/model.gguf`
2. Generate and store the expected model hash (adapter-visible):
   - `python3 scripts/ci/llama_model_hash.py tests/fixtures/llama_cpp_repro/model.gguf --t81-bin build-llama/t81 --out tests/fixtures/llama_cpp_repro/model_hash.txt`
   - Expected format: `sha3-512:<hex>`
3. Enable the CI variable:
   - `T81_ENABLE_LLAMA_REPRO=1`

The CI workflow will only execute llama repro steps when both conditions are
satisfied:

- fixture files exist (`model.gguf`, `model_hash.txt`, `policy.apl`, `prompt.txt`)
- `T81_ENABLE_LLAMA_REPRO == '1'`

`policy.apl` is a template and must include `__MODEL_HASH__`; CI replaces this
placeholder with `model_hash.txt` before running the gate.

## Determinism scope

This gate validates practical repeatability for the governed llama.cpp path.
It does not expand DCP or registry-verified determinism claim boundaries.
