# VM Monolith Decomposition Plan

Date: 2026-02-26  
Scope: `core/vm/vm.cpp`  
Status: In progress (Phase A scaffold completed; Phase B started: scalar value ops extracted to `core/vm/value_ops.cpp`)

## Objective
Reduce regression blast radius and improve assurance depth by decomposing `core/vm/vm.cpp` into testable modules while preserving byte-for-byte runtime behavior on DCP surfaces.

## Constraints
- No opcode semantic changes during extraction phases.
- All existing VM/Axion/trace/determinism tests must remain green at each phase.
- New modules must keep deterministic trap strings and trace reason strings stable.
- Any behavior-changing fixes must land in separate commits after decomposition steps.

## Target Decomposition
1. `vm_dispatch_core.cpp`  
Purpose: opcode dispatch loop, step/run orchestration, trap routing.
2. `vm_memory_segments.cpp`  
Purpose: segment checks, stack/heap/meta helpers, bounds trace emission.
3. `vm_value_ops.cpp`  
Purpose: scalar arithmetic, comparisons, sum-type constructors/unwrappers.
4. `vm_tensor_ops.cpp`  
Purpose: tensor/shape/weights opcode handlers and deterministic guard checks.
5. `vm_policy_bridge.cpp`  
Purpose: Axion syscall context assembly, per-instruction policy checks, fail-closed hooks.
6. `vm_trace_log.cpp`  
Purpose: trace and Axion event recording helpers, deterministic reason formatting.

## Phase Plan
1. Phase A: Extraction scaffolding
- Introduce internal headers under `core/vm/internal/`.
- Move pure helper functions first (no opcode handler movement).
- Add compile-time include boundaries for each helper module.
2. Phase B: Stateless handler extraction
- Move deterministic arithmetic/comparison handlers to `vm_value_ops.cpp`.
- Keep exact control flow and trap code paths.
3. Phase C: Memory and stack extraction
- Move `Load/Store/Push/Pop/Stack*` helpers to `vm_memory_segments.cpp`.
- Preserve segment event strings.
4. Phase D: Policy and trace bridge extraction
- Move policy syscall context assembly + trace emission helpers.
- Add explicit deterministic signature tests for trace reasons.
5. Phase E: Tensor and extended opcode extraction
- Move tensor and weights handlers.
- Keep non-DCP boundaries unchanged.
6. Phase F: Final dispatch slimming
- Reduce `vm.cpp` to dispatch integration and module wiring.

## Verification Gates Per Phase
- `ctest --test-dir build -R "t81_vm_.*|vm_.*|axion_.*|jit_.*|determinism.*|canonfs_.*"`
- `scripts/ci/run_determinism_slice.sh build`
- `python3 scripts/ci/check_tisc_freeze_integrity.py`
- `python3 scripts/governance/check_spec_code_alignment_baseline.py`

## Risk Register
1. Trace reason drift due to string formatting movement.
Mitigation: preserve helper for canonical string assembly and add snapshot assertions.
2. Trap mapping drift due to handler extraction order.
Mitigation: freeze trap expectations with semantic family tests.
3. Hidden coupling between policy checks and dispatch order.
Mitigation: keep per-instruction policy hook call order unchanged and assert via policy matrix tests.
4. Performance regressions from over-fragmentation.
Mitigation: benchmark after each major phase and keep hot helpers inline where needed.

## Exit Criteria
- `core/vm/vm.cpp` reduced to integration layer with materially lower cyclomatic concentration.
- No regressions in determinism and governance gate suites.
- Updated architecture docs reflect new module boundaries with evidence links.
