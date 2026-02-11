# T81 System Integration: Architectural Coalescence

This document provides a comprehensive overview of how the various components of the T81 Foundation stack—T81Lang, TISC, HanoiVM, Axion, and CanonFS—coalesce into a functional, deterministic, and auditable system.

---

## 1. The Vision: A Reproducible Computing Discipline

The T81 stack is not merely a collection of libraries; it is a vertical integration designed to treat **nondeterminism as an engineering attack surface**. The goal is to ensure that a program's behavior is identical regardless of the host architecture, toolchain version, or execution environment.

This is achieved by enforcing strict boundaries and deterministic contracts at every layer of the stack.

---

## 2. The Vertical Stack

The T81 architecture is organized into five functional layers, each building upon the guarantees of the one below.

### Layer 1: The Numeric Substrate (`T81Int`, `T81BigInt`, `T729Tensor`)
At the lowest level, T81 defines canonical balanced ternary arithmetic. By using balanced ternary ({-1, 0, +1}), the system avoids the platform-specific pitfalls of binary floating-point arithmetic (rounding modes, denormals, etc.).
- **Coalescence Role:** Provides the deterministic "physics" of the system. All higher-level operations (like Llama-3.2 inference) rely on these primitives to produce bit-identical results across AVX2-enabled CPUs.

### Layer 2: Storage and Persistence (`CanonFS`)
CanonFS is a content-addressed filesystem where every object is identified by its `CanonHash81` (a 256-bit SHA3-512 truncation).
- **Coalescence Role:** Ensures that code and data (weights) are immutable and verifiable. When the VM loads a program, it isn't just loading a file; it is loading a specific, hashed artifact that is guaranteed to be the correct version.

### Layer 3: Program Representation (`T81Lang`, `TISC`)
T81Lang is a high-level, metadata-preserving language that compiles into TISC (Ternary Instruction Set Computer) intermediate representation.
- **Coalescence Role:** Bridges human intent to machine execution. The compilation process is bit-stable, meaning the same source always produces the same TISC binary, preserving the audit trail from code to execution.

### Layer 4: The Execution Engine (`HanoiVM`, `Trace-JIT`)
HanoiVM is the virtual machine that executes TISC bytecode. It uses a segmented memory model (CODE, STACK, HEAP, TENSOR, META) to prevent memory corruption and ensure isolation.
- **Coalescence Role:** Translates TISC instructions into state changes in the numeric substrate. The **Trace-JIT** optimizes hot paths without sacrificing determinism by only compiling side-effect-free numeric/tensor operations that are subject to Axion boundary checks.

### Layer 5: Governance and Oversight (`Axion`)
Axion is the supervisory kernel that monitors HanoiVM. It evaluates policies written in APL (Axion Policy Language) and produces deterministic trace logs.
- **Coalescence Role:** Acts as the "auditor" of the system. It ensures that the execution adheres to resource limits and safety constraints, providing a verifiable record of *how* a result was achieved.

---

## 3. Functional Coalescence: The Life of a Program

To understand how these components work together, we can follow the lifecycle of a T81 application.

### Phase 1: Source to Binary (Compilation)
A developer writes source code in `.t81`. The `t81 compile` tool invokes the `T81Frontend` (Lexer -> Parser -> Semantic Analyzer -> IRGenerator).
- **Integration Point:** The compiler emits a `.tisc` binary. This binary contains the executable code and structural metadata required for Axion's reflection-based auditing.

### Phase 2: Environment Bootstrapping
Before execution, an `AxionContext` is initialized with a specific **Policy**. This policy might define:
- `max-instructions`: 1,000,000,000
- `require-self-model-integrity`: `true`
- `allowed-segments`: `[CODE, STACK, HEAP, TENSOR]`

### Phase 3: Supervised Execution
HanoiVM starts stepping through the TISC instructions.
- **Integration Point:** For every instruction, Axion checks the policy. If a `TMatMul` instruction is encountered:
    1. HanoiVM requests the tensor handles.
    2. Axion verifies that the handles are within the TENSOR segment.
    3. The `T729Tensor` kernels perform the math using AVX2-accelerated ternary primitives.
    4. Axion records the event in the **Axion Trace**.

### Phase 4: Dynamic Optimization
If a loop is executed multiple times, the **Trace Hotspot Detector** triggers the **Trace-JIT**.
- **Integration Point:** The JIT generates native x86_64 code for the hot path. However, this code includes "guard" instructions that return control to Axion if any boundary (like a memory limit) is approached, maintaining the safety guarantees of the interpreter while approaching native speeds.

### Phase 5: Audit and Replay
Once execution completes, the system outputs the result and an **Axion Trace Log**.
- **Integration Point:** A third party can take the original `.tisc` binary, the same weights from **CanonFS**, and the **Axion Trace** to "replay" the execution. Because the entire stack is deterministic, the replay is guaranteed to match the original execution perfectly, proving that the output was generated according to the specified policy.

---

## 4. Key System Guarantees

The coalescence of these components provides three "Hard Guarantees":

1.  **Bit-Identical Reproducibility:** Identical inputs and policies always produce identical outputs and traces across all supported platforms.
2.  **Policy-Enforced Safety:** No instruction can violate the constraints set in the Axion Policy; the VM will trap deterministically before a violation occurs.
3.  **Auditability of Intent:** By preserving metadata from T81Lang through TISC to the Axion Trace, the system allows auditors to map high-level code logic directly to low-level machine events.

---

## 5. Conclusion: The Unified Machine

The T81 Foundation architecture transforms a collection of isolated tools into a unified, accountable machine. By integrating the numeric precision of ternary math, the structural integrity of CanonFS, and the supervisory governance of Axion, T81 provides a foundation for the next generation of high-assurance and auditable computing.
