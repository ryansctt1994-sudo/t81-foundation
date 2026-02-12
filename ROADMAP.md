# Project Roadmap

> **Source of Truth:** This document defines the **strategic forward-looking milestones** for the project. For current operational status, see [STATUS.md](STATUS.md). For past changes, see [CHANGELOG.md](CHANGELOG.md).

**Last Updated:** February 10, 2026

## 1. Current Phase

T81 is in a **post-v1.0 hardening and scaling phase**:
- Core determinism pipeline is implemented (T81Lang -> TISC -> HanoiVM -> Axion traces).
- CanonFS/Axion runtime surfaces are implemented and covered by regression tests.
- CI enforces cross-arch reproducibility gates and runtime-contract sync checks.

## 2. 2026 Objectives

### P0: Production Reproducibility Surface
- Keep deterministic build/test/repro rituals green by default.
- Maintain cross-arch bit-identity gates for T81Lang and T3_K.
- Keep runtime contract pinning aligned with `t81-vm`.

### P1: Throughput and Latency
- Optimize multi-limb `T81BigInt` arithmetic (SIMD + algorithmic improvements).
- Improve tensor kernel throughput on hot paths.
- Improve CanonFS sustained throughput while preserving deterministic traces.

### P2: Ecosystem Adoption
- Stabilize and expand CLI workflows (`compile`, `run`, `disasm`, `debug`, `trace replay`, `weights`).
- Keep docs/examples synchronized with real command surfaces.
- Expand integration pathways for downstream consumers (`t81-examples`, runtime boundary).

### P3: Verification Expansion
- Extend formal/proof-oriented coverage of arithmetic and policy invariants.
- Expand deterministic replay and trace-equivalence checks.

## 3. Release Discipline

A release candidate is valid only when:
1. `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` succeeds.
2. `cmake --build build --parallel` succeeds.
3. `ctest --test-dir build --output-on-failure` passes fully.
4. Reproducibility and runtime-contract CI gates remain green.

## 4. Living Backlogs

- Near-term actionable work: `TASKS.md`
- Implementation/spec conformance notes: `ANALYSIS.md`
