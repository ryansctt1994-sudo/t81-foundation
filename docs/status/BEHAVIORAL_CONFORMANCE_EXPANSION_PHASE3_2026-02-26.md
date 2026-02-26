# Behavioral Conformance Expansion Phase 3

Date: 2026-02-26  
Status: In progress  
Baseline: `d1cef94c`

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
4. Added deterministic `TLOADHASH` decode-fault matrix coverage:
   - `tests/cpp/vm_tloadhash_decodefault_determinism_matrix_test.cpp`
   - Covers malformed CanonFS tensor objects yielding `DecodeFault`.
5. Added mixed workload conformance matrix coverage:
   - `tests/cpp/vm_mixed_workload_conformance_matrix_test.cpp`
   - Combines policy + tensor + memory + branch + sum-type surfaces in one signature-hardened workload.
6. Continued VM extraction on trace/log boundaries:
   - `core/vm/internal/policy_trace_bridge.hpp`
   - `core/vm/policy_trace_bridge.cpp`
   - `core/vm/vm.cpp`
   - Memory-segment and bounds-fault logging now route through bridge helpers.
7. Expanded `TLOADHASH` deterministic classification matrix:
   - `tests/cpp/vm_tloadhash_decodefault_determinism_matrix_test.cpp`
   - Added explicit deterministic classification coverage for:
     - invalid hash string -> `DecodeFault`
     - CanonFS miss -> `BoundsFault` + `canonfs_miss` reason
     - malformed object decode faults -> `DecodeFault`
8. Expanded mixed workload conformance with deterministic deny-path branch coverage:
   - `tests/cpp/vm_mixed_workload_conformance_matrix_test.cpp`
   - Added low instruction-budget policy case to force deterministic deny during branch loop.

## Next Planned Slice

1. Continue VM integration extraction with opcode-family dispatch splitting to reduce `core/vm/vm.cpp` control concentration.
2. Expand workload-level determinism/conformance cases toward longer multi-op mixes with bounded fault-injection checkpoints.
3. Extend Axion conformance matrices with clause-combination invariants across allow/deny interaction orderings.
