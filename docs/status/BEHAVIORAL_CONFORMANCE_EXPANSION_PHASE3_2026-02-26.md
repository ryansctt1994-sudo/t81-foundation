# Behavioral Conformance Expansion Phase 3

Date: 2026-02-26  
Status: In progress  
Baseline: `838e05f8`

## Objective

Scale conformance from representative matrix slices toward subsystem-complete invariants while continuing VM integration-layer concentration reduction.

## Current Sprint Slice

Completed in this slice:

1. Added deterministic VM trap-family matrix coverage:
   - `tests/cpp/vm_fault_family_determinism_matrix_test.cpp`
   - Families: `DivisionFault`, `BoundsFault`, `TypeFault`, `ShapeFault`, `SecurityFault`
   - Verifies run-to-run trap and signature stability.
2. Added CanonFS read-verify environment contract coverage:
   - `tests/cpp/canonfs_read_verify_env_contract_test.cpp`
   - Verifies env contract across `unset`, `0`, `false`, `OFF`, `1`, and non-empty unknown values.
3. Wired both suites into `CMakeLists.txt` test inventory.

## Next Planned Slice

1. Expand fault-family matrix with `DecodeFault` path determinism on `TLOADHASH` malformed references.
2. Add mixed workload conformance matrix combining policy + tensor + memory + branch families in one deterministic signature harness.
3. Continue VM integration extraction on trace/log helper boundaries to reduce `core/vm/vm.cpp` control concentration.
