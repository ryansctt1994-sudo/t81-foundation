# T81Lang Standard Library Stabilization Plan

Date: 2026-02-26  
Status: Active  
Owner: Language + Governance

## 1. Goal

Stabilize the T81Lang standard library as a governed, deterministic interface
surface suitable for long-lived programs, without broadening determinism claims
beyond DCP/registry boundaries.

## 2. Why This Is Next

Core ISA/VM/governance hardening has reached a point where user-facing
durability is now the highest leverage work. For a ternary deterministic
language, stdlib stability is the bridge from runtime correctness to practical
program construction.

## 3. Scope (This Sprint)

In-scope modules (freeze candidates):

- `std.core`
- `std.math` (bounded deterministic profile)
- `std.io`
- `std.collections`
- `std.text`
- `std.bytes`
- `std.symbol`
- `std.sys`
- `std.async`
- `std.tensor`
- `std.agent`

Out-of-scope:

- introducing new nontrivial module families
- expanding non-DCP claims
- changing VM determinism profile boundaries

## 4. Stabilization Principles

1. Module surface changes require explicit governance records.
2. Determinism claims remain bounded to registry-verified surfaces.
3. Observable behavior is locked by fixture-driven CLI tests and conformance
   suites.
4. Any host-dependent math behavior is explicitly labeled and policy-bounded.
5. Experimental cognitive/agentic surfaces remain marked non-DCP unless promoted.

## 5. Acceptance Gates

1. `scripts/governance/check_stdlib_surface_baseline.py` passes.
2. CLI std fixture suites remain green:
   - `cli_std_text_fixtures_test`
   - `cli_std_bytes_fixtures_test`
   - `cli_std_collections_fixtures_test`
   - `cli_std_tensor_fixtures_test`
   - `cli_std_runtime_fixtures_test`
   - `cli_std_symbol_fixtures_test`
3. `scripts/ci/run_determinism_slice.sh build` remains green.
4. Documentation alignment is maintained between:
   - `docs/standards/standard-library.md`
   - `lang/stdlib/std/*.t81`
   - task/status artifacts
5. No broadening of DCP/verified claims in public docs.

## 6. Work Packages

1. Surface freeze manifest and CI gate
2. Behavioral invariant expansion per stdlib module group
3. Determinism-boundary labeling cleanup in stdlib docs
4. Semver/promotion criteria for stdlib module changes
5. Release-note and control-center synchronization

## 7. Exit Criteria

1. Stdlib surface baseline gate is enforced in CI and governance hygiene.
2. Module coverage is traceable from docs to fixtures/tests.
3. Open stdlib stabilization tasks in `docs/roadmaps-plans/TASKS.md` are closed
   or explicitly deferred with rationale.

