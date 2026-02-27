# Language-Surface Inventory Audit

| Type | Backend Exists | Exposed in T81Lang | VM Opcode Coverage | Canonical Serialization | Determinism Tests | Status |
| :--- | :---: | :---: | :---: | :---: | :---: | :--- |
| **Primitives** | | | | | | |
| `i2` (Trit) | YES | YES | YES | YES | PARTIAL | Stable |
| `i8` (Tryte) | YES | YES | YES | YES | PARTIAL | Stable |
| `i16` | YES | YES | YES | YES | PARTIAL | Stable |
| `i32` | YES | YES | YES | YES | PARTIAL | Stable |
| `T81BigInt` | YES | YES | YES | YES | YES | Stable |
| `T81Float` | YES | YES | YES | YES | YES | Stable |
| `T81Fraction` | YES | YES | YES | YES | PARTIAL | Stable |
| `T81Fixed` | YES | YES | UNKNOWN | UNKNOWN | UNKNOWN | Beta |
| `T81Complex` | YES | YES | UNKNOWN | UNKNOWN | UNKNOWN | Beta |
| `T81Qutrit` | YES | YES | YES | YES | UNKNOWN | Beta |
| `T81Uint` | YES | YES | YES | YES | UNKNOWN | Stable |
| **Collections** | | | | | | |
| `T81String` | YES | YES | YES | YES | YES | Stable |
| `T81Bytes` | YES | YES | YES | YES | YES | Stable |
| `T81Vector` | YES | YES | YES | YES | PARTIAL | Stable |
| `T81Matrix` | YES | YES | YES | PARTIAL | UNKNOWN | Beta |
| `T81Tensor` | YES | YES | YES | PARTIAL | UNKNOWN | Beta |
| `T81List` | YES | NO | NO | NO | NO | Draft |
| `T81Map` | YES | NO | NO | NO | NO | Draft |
| `T81Set` | YES | NO | NO | NO | NO | Draft |
| `T81Tree` | YES | NO | NO | NO | NO | Draft |
| `T81Graph` | YES | PARTIAL | UNKNOWN | NO | NO | Draft |
| **Symbolic** | | | | | | |
| `T81Symbol` | YES | YES | YES | YES | PARTIAL | Stable |
| `T81Symbolic` | YES | PARTIAL | UNKNOWN | UNKNOWN | UNKNOWN | Experimental |
| `T81Polynomial`| YES | PARTIAL | UNKNOWN | UNKNOWN | UNKNOWN | Experimental |
| **System** | | | | | | |
| `Option` | YES | YES | N/A | N/A | N/A | Stable |
| `Result` | YES | YES | N/A | N/A | N/A | Stable |
| `T81Time` | YES | PARTIAL | UNKNOWN | UNKNOWN | UNKNOWN | Experimental |
| `T81Entropy` | YES | PARTIAL | UNKNOWN | UNKNOWN | UNKNOWN | Experimental |
| `T81Promise` | YES | PARTIAL | UNKNOWN | UNKNOWN | UNKNOWN | Experimental |
| `T81Agent` | YES | PARTIAL | UNKNOWN | UNKNOWN | UNKNOWN | Experimental |

## Gaps Identified

1.  **Collections Gap:** `T81List`, `T81Map`, `T81Set`, `T81Tree` exist in backend but are completely missing or just stubs in T81Lang. `T81Graph` is partially exposed via `std.collections.graph` calls but lacks deeper integration.
2.  **Printing/Serialization Gap:** `T81Matrix`, `T81Tensor`, `T81Graph`, `T81Tree` likely lack canonical serialization in the VM/Language layer.
3.  **Math Gap:** `T81Fixed` and `T81Complex` are in the Lexer/Parser but unclear if fully lowered/supported in VM.
4.  **Symbolic Gap:** `T81Symbolic` and `T81Polynomial` are exposed via `std.*` calls but need verification of full lifecycle.

## Plan Refinement

I will focus on "closing what already exists".
The biggest "Backend Exists but not Exposed" offenders are the collections (`Map`, `Set`, `List`, `Tree`).
The `std.collections.*` builtins in `semantic_analyzer.cpp` suggest `Map`, `Set`, `Graph` are handled via intrinsic calls, not necessarily as first-class syntax (e.g. literals).

I need to check `core/vm` to see how `T81Map` and `T81Set` are handled.
