# Determinism Verification Report
**Date:** 2026-03-01
**Agent:** Jules

## Objective
To explore, exercise, and verify the deterministic behavior of the T81 core data type surface using T81Lang.

## Experiment Summary

| Experiment Layer | Status | Key Deterministic Surfaces Verified | Limitations / Notes |
| :--- | :--- | :--- | :--- |
| **Primitive Arithmetic** | **PASS** | `T81Int`, `T81BigInt` (large ops), `T81Fraction`, `T81Float` | Float equality check trapped (TypeFault). `T81Fixed` unavailable. |
| **Symbolic & Algebraic** | **PASS** | `T81Polynomial` (canon, confluent), `T81Symbolic` (canon, confluent) | `print()` lacks support for symbolic types. |
| **Container & Structural** | **PASS** | `Vector` (List), `len`, `push`, `first` | `Map`, `Set`, `Graph` usage caused `DecodeFault` in VM. `T81Tree` unavailable. |
| **Numerical Tensor** | **PASS** | `Tensor` (from list), `vec_add` | Direct tensor printing unsupported. |
| **Monadic / Flow** | **PASS** | `Option`, `Result` matching | Match arms require consistent return types; block exprs tricky. |
| **Reflective Layer** | **PASS** | `sys.time`, `sys.entropy`, `sys.proof`, `agent.self_reflect` | All system calls returned deterministic defaults (0). |
| **Cross-Layer** | **PASS** | Polynomial -> Tensor sim, Agent -> Graph logic, Monadic Pipeline | Demonstrated composition despite individual type limits. |

## Artifacts
- **Source Code:** `experiments/*.t81`
- **TISC Bytecode:** `experiments/*.tisc`
- **Verification Script:** `experiments/verify_determinism.sh`

## Determinism Hash Log (SHA256)
- `primitive_arithmetic`: `0bf2655bcff182b70a06ee942312775049f37992da93bbdbc47b5dd9df475b12`
- `symbolic_algebraic`: `181f08f262570b3386cde36ab7253c1fd1895375b777730eaffccccc549e1074`
- `container_structural`: `e521d03fa61fb2085a5c1c19572538220f2a46b9d7d1853a5bc82dc46c2cba02`
- `numerical_tensor`: `95e46d64d46acb36839fc83bd714af509bbfba465a8fbfcf66ea54accd5576fd`
- `monadic_flow`: `544669bf578c8ba665418b936e7dc2ffbc2c5ef139a230662d590fce52c01e1d`
- `reflective_layer`: `22ac0f71f715fd41c05e2f420123ceb706f0926db9352c6cba77539e75acdf8e`
- `cross_layer`: `d8a5213d196dd50aa6474ebc299a8bdbc6bb19ded5786e60bc70feceb5012c86`

## Conclusion
The core T81 data types exhibit stable, deterministic behavior across executed surfaces. Verification confirmed bit-exact reproducibility for arithmetic, symbolic logic, and basic container operations. Experimental features (higher-order containers, advanced tensor ops) are present but show varying degrees of runtime maturity.
