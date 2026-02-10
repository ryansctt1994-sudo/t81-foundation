# T81 Foundation Whitepaper

## Introduction

T81 Foundation is a ternary-native deterministic computing stack built to make
model/runtime behavior auditable and reproducible across environments.

Core proposition:
- deterministic numerics and execution,
- canonical binary/runtime artifacts,
- Axion policy enforcement with replayable traces,
- explicit provenance contracts for runtime boundaries.

## Current Architecture Surface

1. **Core Numerics** (`include/t81/core`, `src/`)
   - Balanced ternary data types and deterministic arithmetic foundations.
2. **Frontend** (`include/t81/frontend`, `src/frontend`)
   - Lexer/parser/semantic analysis/IR generation for T81Lang.
3. **TISC + VM** (`include/t81/tisc`, `src/tisc`, `src/vm`)
   - Canonical bytecode representation and deterministic execution runtime.
4. **Axion + CanonFS** (`src/axion`, `src/canonfs`)
   - Policy and persistence layers tied to deterministic trace semantics.
5. **CLI + Tooling** (`src/cli`, `docs/guides`, `tests/cpp`)
   - Compile/run/disasm/debug/replay + reproducibility verification workflows.

## Determinism and Verification

The project enforces a repeatable ritual for local and CI confidence:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Latest local baseline (single-threaded safe run): **139/139 tests passed**.

Cross-arch reproducibility and runtime-contract sync are enforced in CI via
workflow gates documented in `docs/ci.md`.

## Runtime Boundary Model

T81 separates semantic governance from runtime compatibility artifacts:
- `t81-foundation` owns semantic/source-of-truth behavior.
- `t81-vm` owns runtime compatibility artifacts and host ABI contract lane.
- Boundary sync is pinned by `contracts/runtime-contract.json` + CI checks.

## Near-Term Focus

- Throughput improvements in BigInt/tensor/CanonFS hot paths.
- Continued expansion of deterministic verification depth.
- Ecosystem hardening for reproducible downstream consumption.

## Conclusion

T81’s differentiator is not only ternary representation; it is a governance +
engineering model where determinism, replayability, and explicit contracts are
first-class. The stack is operational today and in active hardening for scale.
