# Active Development Tasks

**Last Updated:** February 26, 2026

This document tracks immediate, actionable tasks for the T81 project.

## 1. Current Sprint: March Governance Close + Governed Inference

### Release and Governance Closure (C2)
- [ ] **C2 Month-Close:** Execute runbook on 2026-03-31 and stamp final outcomes in `docs/records/audits/2026-03-governance-review.md`.
- [x] **Release Candidate Decision:** Select March release-candidate SHA and record required-context results in `docs/status/RELEASE_READINESS_PACKET_2026-03.md`. (Completed 2026-02-26: candidate `b4fdf8efdf249e391f7c93fb18cf9245926b6a38` recorded with required-context evidence; decision remains `HOLD` pending `quality gate / required` and `Analyze (cpp)` success on a post-fix main candidate.)
- [x] **Promotion Snapshot Refresh:** Re-run `python3 scripts/governance/t81lang_promotion_gate_snapshot.py` within close window and capture result. (Completed 2026-02-26; snapshot refreshed as `READY` in `docs/status/T81LANG_PROMOTION_GATE_SNAPSHOT.md` and recorded in March packet/audit artifacts.)
- [x] **Status Integrity:** Re-run governance hygiene and status link-target checks and record pass/fail evidence. (Completed 2026-02-26; hygiene check and link-target sweep passed, recorded in `docs/status/RELEASE_READINESS_PACKET_2026-03.md` and `docs/records/audits/2026-03-governance-review.md`.)

### Governed llama.cpp Integration
- [x] **Fixture Pair:** Add one sanctioned model/policy fixture pair for local/CI reproducibility gate execution. (Completed 2026-02-26: CI provisioning path added in `.github/workflows/ci.yml` with sanctioned-source controls `T81_LLAMA_MODEL_URL`, `T81_LLAMA_EXPECTED_MODEL_HASH`, optional `T81_LLAMA_MODEL_SHA256`; fixture policy documented in `tests/fixtures/llama_cpp_repro/README.md`.)
- [x] **CI Gate Wiring:** Add guarded CI path for `scripts/ci/llama_cpp_repro_gate.py` when fixtures are present. (Completed 2026-02-26; optional fixture-gated steps added to `.github/workflows/ci.yml`.)
- [x] **Packaging Decision:** Decide whether `t81_llama_adapter` remains internal or is exported as a package target. (Completed 2026-02-26: remains internal/build-only; codified by `T81_EXPORT_LLAMA_ADAPTER` guard in `CMakeLists.txt` with explicit unsupported-error path.)
- [x] **Boundary Classification:** Keep llama integration explicitly classified as governed non-DCP in status/release artifacts. (Completed 2026-02-26; explicit classification added in `docs/status/RELEASE_READINESS_PACKET_2026-03.md` and `docs/status/IMPLEMENTATION_MATRIX.md`.)

### Experimental Implementation Backlog (Post-C2 Pickup)
- [ ] **Cognitive Tier 1:** Implement `SymbolicGraph::rewrite` and `is_confluent`.
- [ ] **Cognitive Tier 2:** Connect `ReflectiveFrame` to Axion trace events.
- [ ] **Cognitive Tier 3:** Implement `Recursor` evaluation loop and depth proof verification.
- [ ] **Cognitive Tier 4:** Implement `NodeState` synchronization and gossip protocol logic.
- [ ] **Cognitive Tier 5:** Implement `InfiniteCanonicalForm` lazy expansion logic.
- [ ] **Runtime JIT:** Advance `runtime/jit` from experimental research to prototype backend.
- [ ] **CanonFS:** Optimize `PersistentDriver` for high-throughput tensor I/O.
- [ ] **CLI Tooling:** Add `t81 trace export` for Axion logs (JSON/CSV).
- [ ] **Debugger Tooling:** Extend `t81 debug` with cognitive tier state inspection.

______________________________________________________________________

## 2. Recently Completed (Feb 2026)

The highest-impact closures for this cycle are complete and tracked in:

1. `docs/status/T81LANG_IMPLEMENTATION_TASK_QUEUE_2026-03.md` (A1 through A1G closures)
2. `docs/status/T81LANG_ENGINEERING_BACKLOG_2026-03.md` (BG-01 through BG-05 closures)
3. `docs/status/EXECUTION_PLAN_2026-03.md` (A1/A2/A3/B1/B2/B3/C1/C3/D1 completed; C2 in progress)

For historical implementation detail, see `docs/records/audits/2026-03-governance-review.md`.
