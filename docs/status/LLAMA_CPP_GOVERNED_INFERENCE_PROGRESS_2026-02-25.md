# Llama.cpp Governed Inference Progress (2026-02-25)

Status: In Progress
Last Updated: 2026-02-25
Owner: t81dev

## Objective

Stand up a governed `llama.cpp` integration path in T81 aligned with deterministic ternary
architecture direction, while classifying this surface as governed non-DCP.

## Completed This Session

1. Vendored dependency location established:
   - `third_party/llama.cpp`
   - pinned commit: `3769fe6eb70b0a0fbb30b80917f1caae68c902f7`

2. Build integration added behind feature gate:
   - `-DT81_ENABLE_LLAMA_CPP=ON`
   - embedded `llama.cpp` via CMake `add_subdirectory`
   - disabled `llama.cpp` tools/examples/tests for minimal library embedding

3. New adapter library implemented:
   - `t81_llama_adapter`
   - API: `include/t81/experimental/llama_cpp_adapter.hpp`
   - Impl: `tooling/model/llama_cpp_adapter.cpp`
   - Axion policy is required and evaluated before inference
   - model hash and prompt hash are emitted in receipt
   - model file hashing uses streaming SHA3-512 (no full-file memory load)

4. New demo target added:
   - `llama_cpp_governed_demo`
   - source: `examples/llama_cpp_governed_demo.cpp`

5. CLI command added:
   - `t81 llama-run <model.gguf> <prompt> --policy <policy.apl> [options]`
   - deterministic defaults: `temperature=0`, `top_k=1`, `top_p=1`, `threads=1`, `seed=0`
   - emits `token_ids_csv` for reproducibility checks
   - now accepts CanonFS model hash input: `sha3-256:<hash>`
   - new option: `--canonfs-root <path>` for hash-based model loading

6. New raw CanonFS ingestion command:
   - `t81 canonize-file <file> [--canonfs-root <path>]`
   - stores raw file bytes in CanonFS and prints `sha3-256:<hash>`

7. Reproducibility gate script added:
   - `scripts/ci/llama_cpp_repro_gate.py`
   - runs `t81 llama-run` repeatedly and fails on drift in:
     - `model_hash`
     - `prompt_hash`
     - `token_ids_csv`

8. Optional CTest hook added:
   - `llama_cpp_repro_gate_test`
   - only registered when all three are set at configure time:
     - `T81_LLAMA_REPRO_MODEL`
     - `T81_LLAMA_REPRO_POLICY`
     - `T81_LLAMA_REPRO_PROMPT`

9. Governance docs updated:
   - new pin file: `docs/governance/EXTERNAL_DEPENDENCY_PINS.md`
   - governance index updated to include dependency pin doc

## Verified Commands

```bash
cmake -S . -B build-llama -DT81_ENABLE_LLAMA_CPP=ON -DT81_BUILD_TESTS=OFF -DT81_BUILD_BENCHMARKS=OFF -DT81_BUILD_FUZZ_TESTS=OFF
cmake --build build-llama --target t81_llama_adapter llama_cpp_governed_demo -j4
cmake --build build-llama --target t81 -j4
./build-llama/t81 help llama-run
python3 scripts/ci/llama_cpp_repro_gate.py --help
```

All commands above succeeded in this workspace.

## Known Gaps vs AGI How-To Intent

1. This is practical reproducibility, not DCP-level cross-platform determinism.
2. `t81_llama_adapter` is currently build-only (not exported/installed target).
3. No checked-in real model/policy fixture pair yet for automated gate execution in CI.

## Current Working Tree Impact

Tracked changes:
- `CMakeLists.txt`
- `tooling/cli/main.cpp`
- `docs/governance/README.md`
- `scripts/ci/README.md`

New files:
- `include/t81/experimental/llama_cpp_adapter.hpp`
- `tooling/model/llama_cpp_adapter.cpp`
- `examples/llama_cpp_governed_demo.cpp`
- `scripts/ci/llama_cpp_repro_gate.py`
- `docs/governance/EXTERNAL_DEPENDENCY_PINS.md`
- `docs/status/LLAMA_CPP_GOVERNED_INFERENCE_PROGRESS_2026-02-25.md`

Untracked dependency directory:
- `third_party/llama.cpp`

## Recommended Next Pickup Steps

1. Add one sanctioned model/policy fixture pair for local/CI reproducibility gate.
2. Decide whether `t81_llama_adapter` should be exported as package target or remain internal.
3. Extend CI workflow(s) with guarded llama repro execution when fixtures are available.

## Versioning Statement

Progress note only. This file does not alter specification authority or DCP scope.
