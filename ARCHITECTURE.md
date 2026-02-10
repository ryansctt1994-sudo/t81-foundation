# T81 Foundation: Architecture Overview

This document maps the current T81 codebase architecture: build graph, runtime flow, verification surfaces, and cross-repo runtime boundary contracts.

______________________________________________________________________

## 1. Guiding Principles

- **Spec semantics are authoritative:** `/spec` defines normative behavior. If implementation diverges, resolve in favor of spec semantics.
- **Determinism first:** all compiler/runtime paths must preserve reproducible outputs and auditable traces.
- **Layered composition via CMake:** components are separated by responsibility and linked through explicit target dependencies.
- **Optimization without semantic drift:** interpreter, trace-JIT, SIMD, and tooling must preserve canonical behavior.

______________________________________________________________________

## 2. Build Graph (Authoritative Targets)

The authoritative build graph is `CMakeLists.txt`. It includes static libraries, interface libraries, executables, tests, optional Python bindings, and optional benchmarks.

| Target | Kind | Responsibilities | Depends On |
| --- | --- | --- | --- |
| `t81_core` | STATIC | Core numerics, VM runtime, JIT compiler, Axion engine, CanonFS, codecs, hashing/crypto, weights internals | (none) |
| `t81_io` | STATIC | Tensor/model I/O helpers | `t81_core` |
| `t81_c_api` | STATIC | C ABI surface for selected runtime/core functions | `t81_core` |
| `t81_frontend` | STATIC | Lexer, parser, semantic analyzer (T81Lang frontend) | `t81_core` |
| `t81_tisc` | STATIC | TISC IR/binary emitter, pretty printer, binary I/O, base81 TISC views | `t81_core` |
| `t81_vm` | INTERFACE | VM public facade target for consumers/tests | `t81_core` |
| `t81_llvm` | INTERFACE | LLVM-facing facade target (placeholder/adapter layer) | `t81_core` |
| `t81_cli_driver` | STATIC | CLI orchestration (compile/run/trace/repro/tools) | `t81_frontend`, `t81_tisc`, `t81_vm` |
| `t81` | EXECUTABLE | Main CLI entry point | `t81_frontend`, `t81_tisc`, `t81_vm`, `t81_cli_driver` |
| `t81_python` | MODULE (optional) | `pybind11` Python bindings | `t81_core`, `t81_frontend`, `t81_tisc` |
| `benchmark_runner` (subdir) | EXECUTABLE (optional) | Benchmark suite and docs benchmark generation pipeline | `t81_core`, `t81_frontend`, `t81_tisc`, Google Benchmark |

Notes:
- CMake defaults to `cxx_std_23`; a temporary compatibility lane remains available via `-DT81_USE_CXX23=OFF`.
- CMake is the only supported authoritative build surface in this repository.

______________________________________________________________________

## 3. Concurrent Workstream View

The repository is developed as multiple active streams that share deterministic contracts and CI gates.

```mermaid
graph LR
    subgraph A["Language + VM Stream"]
        A1["T81Lang Frontend"]
        A2["TISC Tooling"]
        A3["HanoiVM + Trace-JIT"]
        A1 --> A2 --> A3
    end

    subgraph B["Numerics + Runtime Substrate"]
        B1["T81Int / T81Float / T81BigInt"]
        B2["Tensor + CanonFS"]
        B3["Axion Safety Engine"]
        B1 --> B2 --> B3
    end

    subgraph C["Model + Quantization Stream"]
        C1["weights import/info"]
        C2["T3_K Quantization + Policy Gates"]
        C3["GGUF / t81w Artifacts"]
        C1 --> C2 --> C3
    end

    subgraph D["Verification + Governance"]
        D1["CTest + Fuzz/Property"]
        D2["Repro Gates (T3_K, T81Lang)"]
        D3["Runtime Contract Sync"]
        D4["Spec + RFC Governance"]
        D1 --> D2 --> D3 --> D4
    end

    A3 --> D1
    B3 --> D1
    C3 --> D2
    D4 --> A1
    D4 --> B1
    D4 --> C1
```

How to read this:
- `Language + VM` produces and executes deterministic bytecode.
- `Numerics + Runtime` defines canonical arithmetic/tensor behavior and policy enforcement.
- `Model + Quantization` produces reproducible model artifacts (`t81w` and GGUF/T3_K).
- `Verification + Governance` continuously validates all streams and feeds requirements back into implementation.

______________________________________________________________________

## 4. End-to-End Flow

```mermaid
graph TD
    subgraph Language Toolchain
        A["T81Lang Source (.t81)"] --> B["Lexer"]
        B --> C["Parser"]
        C --> D["AST"]
        D --> E["SemanticAnalyzer"]
        E --> F["IRGenerator"]
        F --> G["TISC IR"]
        G --> H["BinaryEmitter / BinaryIO"]
        H --> I["TISC Program / Bytecode"]
    end

    subgraph Runtime
        I --> J["HanoiVM Interpreter"]
        J --> K["Trace Hotspot Detection"]
        K --> L["Trace JIT Compile (deterministic)"]
        L --> M["Compiled Trace Execute"]
    end

    subgraph Safety and Audit
        J --> N["Axion Policy Checks"]
        M --> N
        N --> O["Axion Events / Reasons / Verdicts"]
    end

    subgraph Model and Tensor Tooling
        P["safetensors / gguf / t81w"] --> Q["weights tooling"]
        Q --> R["tensor pools / handles"]
        R --> J
    end
```

Primary stages:
1. Frontend compiles `.t81` source to validated TISC IR.
2. TISC layer serializes IR to deterministic bytecode.
3. HanoiVM executes via interpreter, with trace-JIT on hot deterministic paths.
4. Axion enforces policy and records boundary/fault events for replay and audit.
5. Weights/tensor tooling feeds model tensors into runtime via canonical handles.

______________________________________________________________________

## 5. Determinism and Verification Plane

Architecture is enforced by automated gates, not just design intent:

- **Build + full test ritual:** CMake + CTest matrix in `CMakeLists.txt`.
- **Extended fuzz/property/Axion checks:** optional but standard pre-release validation.
- **Cross-arch reproducibility gates:** T3_K and T81Lang hash gates in CI.
- **Repro ledger workflow:** scheduled artifact generation for reproducibility evidence.
- **Runtime contract sync checks:** script-backed verification against runtime boundary pins.

Operational sources:
- `docs/ci.md`
- `.github/workflows/ci.yml`
- `.github/workflows/repro-ledger.yml`
- `scripts/ci/t3k_repro_gate.py`
- `scripts/ci/t81lang_repro_gate.py`
- `scripts/ci/check_architecture_targets.py`
- `scripts/check-runtime-contract-sync.py`

______________________________________________________________________

## 6. Runtime Contract Boundary

T81 uses an explicit cross-repo runtime semantics boundary:

- **`t81-foundation` owns:** normative semantics, language/ISA intent, architectural invariants.
- **`t81-vm` owns:** executable runtime compatibility artifacts and VM host ABI contract.
- **Pinned contract marker:** `contracts/runtime-contract.json`.
- **Boundary policy document:** `docs/runtime-semantics-boundary.md`.

This split keeps semantic governance stable while allowing runtime implementation to evolve under explicit compatibility contracts.

______________________________________________________________________

## 7. Key Architectural Boundaries

- **Frontend vs Runtime:** `t81_frontend` produces typed/validated IR; it does not execute programs.
- **TISC Format vs VM Execution:** `t81_tisc` defines program representation; `t81_vm` executes it via interpreter and trace-JIT.
- **Core as substrate:** `t81_core` provides shared primitives/services (VM state, Axion, CanonFS, codecs, tensor numerics) and is the dependency root.
- **Policy vs performance:** Axion policy checks and trace logging remain mandatory across interpreter and compiled trace paths.
- **Model tooling vs execution:** `weights` tooling transforms/loads tensors; HanoiVM consumes handles without changing provenance semantics.

______________________________________________________________________

## 8. Architecture Drift Controls

- **Target-table sync gate:** `scripts/ci/check_architecture_targets.py` verifies that this document's build-target table matches `CMakeLists.txt`.
- **Runtime boundary sync gate:** `scripts/check-runtime-contract-sync.py` verifies runtime contract pinning and policy documents are coherent.
- **Cross-arch reproducibility gates:** CI compares deterministic artifacts across architecture lanes for T81Lang and T3_K workflows.

These controls are required to keep architecture documentation operationally accurate rather than aspirational.

______________________________________________________________________

## 9. Near-Term Architecture Work (Open)

Open architecture-level items remain tracked in `TASKS.md` and `TODO.md`. Current active streams:

- **Deterministic trace-JIT MVP hardening:** side-effect-free numeric/tensor hot-path compilation with Axion boundary checks.
- **BigInt performance path:** SIMD/Karatsuba-oriented multi-limb optimizations without changing canonical arithmetic semantics.
- **CanonFS scalability path:** higher-throughput persistence and retrieval while preserving deterministic traceability.
- **Distributed tensor path:** sharding/runtime orchestration for higher-rank tensor workloads under deterministic replay constraints.
- **Formal verification path:** proofs for core balanced ternary arithmetic primitives and policy-safety envelopes.

______________________________________________________________________

## 10. Local Verification Ritual (Single-Threaded Safe Mode)

When host stability is constrained, run the required ritual in single-threaded mode:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 1
ctest --test-dir build --output-on-failure -j1
```

Optional extended suite (single-threaded):

```bash
ctest --test-dir build -R "fuzz|property|axion" --schedule-random -j1
```
