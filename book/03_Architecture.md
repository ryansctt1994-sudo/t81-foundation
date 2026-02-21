# Chapter 3: Architecture and Core Components

The T81 architecture is designed as a strict pipeline: **Source (T81Lang) $\to$ IR (TISC) $\to$ Execution (T81VM) $\to$ Supervision (Axion)**.

## 3.1 T81Lang: The High-Level Language

T81Lang is the primary interface for developers. It is a statically-typed, safety-first language that enforces the project's ternary and deterministic principles.

### 3.1.1 Core Features
*   **Purity by Default**: Functions are assumed to be pure (no side effects) unless annotated with `@effect`. This allows for aggressive optimization and easier reasoning.
*   **Ternary Primitives**: The language natively understands `T81Int` (arbitrary precision), `T81Float` (canonical float), and `T81Fraction` (rational).
*   **Structural Types**:
    *   `Option[T]`: Represents a value that may or may not exist (`Some(v)` / `None`). Null pointers do not exist.
    *   `Result[T, E]`: Represents success or failure (`Ok(v)` / `Err(e)`). Exceptions do not exist; errors are values.
*   **Pattern Matching**: `match` expressions allow for exhaustive handling of structural types and enums.

### 3.1.2 Compilation
The T81Lang compiler operates in stages:
1.  **Lexing**: Converts source text into canonical tokens.
2.  **Parsing**: Builds an Abstract Syntax Tree (AST).
3.  **Semantic Analysis**: Checks types, scopes, and purity constraints.
4.  **IR Generation**: Produces a ternary-native intermediate representation.
5.  **TISC Emission**: Outputs the final bytecode.

## 3.2 TISC: Ternary Instruction Set Computer

TISC is the target architecture for the compiler. It is an abstract machine designed for determinism.

### 3.2.1 Machine Model
The TISC state is defined as `STATE = (R, PC, SP, FLAGS, MEM, META)`.
*   **Registers (`R`)**: 81 general-purpose registers (`R0`–`R80`).
    *   `R0`: Hardwired Zero.
    *   `R1-R74`: General Purpose.
    *   `R75-R80`: Privileged Axion System Window.
*   **Program Counter (`PC`)**: Points to the next instruction.
*   **Stack Pointer (`SP`)**: Points to the top of the current stack frame.
*   **Flags**: Capture the result of comparisons (Negative, Zero, Positive).

### 3.2.2 Instruction Encoding
Instructions are fixed-width (81 trits).
Format: `[ OPC | MODE | RD | RS1 | RS2 | IMM ]`

### 3.2.3 Opcode Classes
*   **Arithmetic**: `ADD`, `SUB`, `MUL`, `DIV`, `MOD`, `NEG`.
*   **Floating Point**: `FADD`, `FSUB`, `FMUL`, `FDIV` (using handles to the Float Pool).
*   **Logic**: `TNOT`, `TAND`, `TOR` (Ternary Min/Max/Neg).
*   **Control Flow**: `JMP`, `JZ` (Jump Zero), `JN` (Jump Negative), `CALL`, `RET`.
*   **Data Movement**: `MOV`, `LOAD`, `STORE`, `PUSH`, `POP`.
*   **System**: `HALT`, `TRAP`, `AXREAD`, `AXSET` (Privileged).

## 3.3 T81VM: The Virtual Machine

The T81VM is the runtime environment.

### 3.3.1 Memory Segments
Memory is strictly segmented to prevent corruption and ensure safety:
1.  **CODE**: Read-only instructions.
2.  **STACK**: Call frames and local variables.
3.  **HEAP**: Dynamically allocated objects (managed by GC).
4.  **TENSOR**: Dedicated high-performance storage for large numerical arrays.
5.  **META**: Axion metadata and execution traces.

### 3.3.2 Execution Modes
*   **Interpreter**: The reference implementation. Fetches instructions one by one, decodes them, and executes the logic defined in `spec/tisc-spec.md`.
*   **Trace-JIT (Experimental)**: Detects "hot" loops or paths and compiles them to native machine code. Crucially, the JIT must preserve *exact* behavioral equivalence to the interpreter, including all side effects and fault conditions.

### 3.3.3 Garbage Collection
The VM uses a **Mark-and-Sweep** collector.
*   **Roots**: Registers and Stack.
*   **Determinism**: The GC must trigger deterministically (e.g., based on allocation count, not wall-clock time) so that memory addresses and heap layouts remain identical across runs.

## 3.4 Axion Policy Engine

Axion is the "conscience" of the machine. It enforces the "Constitution" at runtime.

### 3.4.1 Responsibilities
*   **Safety**: Preventing buffer overflows, stack overflows, and illegal memory access.
*   **Resource Limits**: Enforcing quotas on instructions executed (`max-instructions`) and recursion depth (`max-recursion`).
*   **Ethics**: Enforcing high-level policies (e.g., "Do not execute code from untrusted sources" or "Do not perform I/O without a capability").

### 3.4.2 Trace System
Axion records a log of significant events:
*   Memory writes (canonicalized).
*   Control flow changes (calls/jumps).
*   Faults and traps.
*   Privileged instruction execution.

This trace is the "proof of execution".

## 3.5 Data Types and Canonicalization

T81 mandates **Canonical Forms** for all data types. A value must have exactly one binary representation.

*   **T81Int**: Base-81 integers. No leading zeros allowed (except for the value 0).
*   **T81Fraction**: Rational numbers. Must be stored in lowest terms (GCD reduced). Denominator must be positive.
*   **T81Float**: Base-81 floating point. Mantissa and exponent must be normalized.
*   **T81Tensor**: N-dimensional arrays. Shapes are immutable.

## 3.6 Model Tooling

To support AI workloads, T81 includes a robust set of tools (`src/tools/weights.cpp`) for handling neural network weights.

*   **Import**: Supports industry-standard formats like `SafeTensors` and `GGUF`.
*   **T81W Format**: The native T81 weights format (`.t81w`). It supports both raw balanced ternary data and quantized formats (`T3_K`).
*   **Quantization**: The `T3_K` format is optimized for ternary storage, compressing weights into blocks with scaling factors.
