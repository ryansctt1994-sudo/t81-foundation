# T81 Foundation: Actionable Task List

**Last Updated:** February 10, 2026

This document tracks **active, near-term execution tasks**. Completed historical work is summarized in `CHANGELOG.md` and supporting docs.

______________________________________________________________________

## P0 — Keep Determinism Gates Green

- [ ] Re-run and document the local ritual on every significant merge window:
  - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`
  - `cmake --build build --parallel`
  - `ctest --test-dir build --output-on-failure`
- [ ] Keep cross-arch T81Lang and T3_K reproducibility gates passing in CI.
- [ ] Keep runtime-contract sync checks green against `t81-vm`.

## P1 — Performance Path

- [x] Implement next-step `T81BigInt` acceleration (SIMD/Karatsuba path), preserving canonical behavior.
- [ ] Profile and optimize hot tensor kernels used by demo/inference paths.
- [ ] Improve CanonFS performance under sustained write/read workloads while preserving deterministic trace strings.

## P2 — Tooling and UX

- [ ] Tighten CLI ergonomics/documentation parity for `disasm`, `debug`, and `trace replay` workflows.
- [ ] Expand deterministic failure diagnostics for compile/run workflows.
- [ ] Keep `examples/` runnable and aligned with docs (including `examples/tisc/` assets).

## P3 — Verification and Hardening

- [ ] Expand property/fuzz coverage for frontend + IR + VM boundary invariants.
- [x] Implement deterministic Gumbel noise for `T81Prob` to support sampling.
- [ ] Add additional parity checks for backend variants against deterministic scalar references.
- [ ] Continue Axion policy/trace regression growth for guard/segment/match paths.

______________________________________________________________________

## Completed Highlights (Moved)

Major completed streams (compiler conformance, VM memory model, Axion/CanonFS integration, C++23 default lane) are now tracked in:
- `CHANGELOG.md`
- `ANALYSIS.md`
- `docs/system-status.md`
