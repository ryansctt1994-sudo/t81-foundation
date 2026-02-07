# Project Roadmap

**Last Updated:** December 4, 2025

## 1. Vision for v1.0

The v1.0 release of the T81 Foundation will deliver a stable, documented, and production-ready implementation of the core T81 stack. The target audience is systems developers and AI/PL researchers who need a deterministic, auditable platform for advanced computation.

"Done" for a v1.0 component means it is fully implemented per the spec, comprehensively tested, documented, and builds cleanly.

## 2. Current Status (v1.0 Candidate)

The project has reached v1.0 feature completeness. The T81Lang compiler is stable, and the runtime environment (HanoiVM, Axion, CanonFS) is fully implemented and hardened. All 90 canonical data types are present and conform to the balanced ternary specifications. The system produces deterministic, auditable Axion traces for all operations, enabling full system verification.

- **Strengths:** End-to-end deterministic stack from T81Lang source to disk-backed CanonFS persistence. Robust multi-limb BigInt and high-rank tensor numerics. Comprehensive documentation and specification examples.
- **Weaknesses:** None identified for v1.0; future work will focus on performance optimization and scaling.

For a detailed breakdown, see the [**System Status Report**](docs/system-status.md) and [`ANALYSIS.md`](ANALYSIS.md).

## 3. Strategic Priorities

With the core stack stabilized, priorities shift toward ecosystem growth and high-tier cognitive applications.

- **[P0] Ecosystem & Tooling:** [IN PROGRESS] HanoiVM debugger and basic package manager implemented. VS Code extension scaffolded.
- **[P1] High-Tier Cognition:** [IN PROGRESS] Tier 3, 4, and 5 cognitive layers defined and integrated into the promotion engine.
- **[P2] Performance Optimization:** Target specific numeric and I/O bottlenecks to improve throughput for large-scale ternary workloads.

A detailed list of actionable tasks for these workstreams can be found in [`TASKS.md`](TASKS.md).

## 4. Release Phases

### Phase 1: Compiler Feature Completeness

- **Goal:** Ship a spec-compliant T81Lang compiler with rich diagnostics and the `t81` CLI experience.
- **Exit Criteria:** The compiler parses and type-checks the entire grammar, includes coverage for `loop`/`match` control flow, and contributes end-to-end tests for each major language feature. Compiler builds must succeed via `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` and `cmake --build build --parallel` before any release candidate.

### Phase 2: Runtime Hardening & Integration (Complete)

- **Goal:** Harden the VM and integrate runtime safety layers such as Axion while validating through the required `ctest --test-dir build --output-on-failure`.
- **Exit Criteria:** The HanoiVM memory model is complete, Axion kernel/CanonFS enforce deterministic safety (instruction counters, recursion guards), and the VM passes deterministic fault and stress tests.

### Phase 3: Public v1.0 Release

- **Goal:** Lock down documentation, tutorials, release scripts, and contributions so the stack is ready for external adoption.
- **Exit Criteria:** All v1.0 Definition of Done items are complete, documentation is aligned with the latest implementation, and the release checklist (build/tests/ctest runs) has been executed on every candidate.
