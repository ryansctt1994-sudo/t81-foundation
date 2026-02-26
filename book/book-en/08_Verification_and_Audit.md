# Chapter 8: Verification and Audit

## 8.1 Verification Methodology

**Status: Active and Enforced**

T81 verification is split between implementation tests, deterministic repro gates, and governance evidence.

### 8.1.1 Verification Layers

1.  **Static and semantic validation**: C++ compile checks plus T81Lang parser/semantic analyzer checks.
2.  **Unit/integration tests**: C++ tests for VM, ISA, numerics, Axion, and CanonFS surfaces.
3.  **Property and determinism checks**: deterministic fixtures and cross-surface invariants.
4.  **Governance controls**: freeze-integrity checks, determinism registry discipline, and release packet evidence.

## 8.2 Determinism Scope and Audit Matrix

Determinism claims are bounded. The authoritative source is the Determinism Surface Registry and DCP definition, not this narrative chapter.

| Surface | Reference | Verification | Current Claim |
| :--- | :--- | :--- | :--- |
| TISC opcode semantics | `spec/tisc-spec.md` | `tests/cpp/vm_determinism_property_test.cpp`, `tests/cpp/test_tritwise_backend_equivalence.cpp` | Verified |
| VM interpreter execution | `spec/t81vm-spec.md` | `tests/cpp/vm_trace_test.cpp`, `tests/cpp/vm_determinism_property_test.cpp` | Verified |
| Data type canonical encoding | `spec/t81-data-types.md` | `tests/cpp/v1_canonical_numeric_contract_test.cpp`, `tests/cpp/tisc_binary_io_determinism_test.cpp` | Verified |
| Soft-float deterministic math | `spec/t81-data-types.md` | `tests/cpp/test_T81Float_arithmetic.cpp`, `tests/cpp/test_T81Float_rounding.cpp` | Verified |
| T81Lang compiler emission | `docs/governance/DETERMINISM_SURFACE_REGISTRY.md` | `scripts/ci/t81lang_repro_gate.py` | Partial |

## 8.3 Reproducibility Gates

**Status: Critical**

### 8.3.1 T81Lang Repro Gate

The `t81lang_repro_gate.py` script compiles and executes canonical fixtures, then validates aggregate hash output.

```bash
python3 scripts/ci/t81lang_repro_gate.py \
  --t81-bin ./build/t81 \
  --fixtures-dir tests/fixtures/t81lang_determinism \
  --workdir build/t81lang-repro-check \
  --hash-out build/t81lang-repro-check/hash.txt \
  --expected-hash-file tests/fixtures/t81lang_determinism/t81lang_repro_hash.txt
```

### 8.3.2 Freeze and Slice Checks

For release hardening, run freeze and determinism slice checks in addition to baseline tests.

```bash
python3 scripts/ci/check_tisc_freeze_integrity.py
scripts/ci/run_determinism_slice.sh
```

### 8.3.3 Governed llama.cpp Repro Gate (Non-DCP)

Governed inference has a dedicated experimental repro path:

```bash
python3 scripts/ci/llama_cpp_repro_gate.py --help
```

This gate is governance-facing and does not expand DCP guarantees.

## 8.4 Failure Implication and Response

A regression on a verified determinism surface is a release blocker and governance incident candidate. The required response path is defined in `docs/governance/INCIDENT_RESPONSE.md` and freeze enforcement policy.
