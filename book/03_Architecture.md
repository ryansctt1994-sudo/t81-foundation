# Chapter 3: T81VM Architecture

## 3.1 Formal State Machine

The T81 Virtual Machine (T81VM) is formally defined as a state transition system $M = (S, \delta)$, where $S$ is the set of valid states and $\delta: S \to S \cup \{\bot\}$ is the transition function.

### 3.1.1 State Definition
The state $S$ is a tuple:
$$S = (R, PC, SP, M_{stack}, M_{heap}, M_{tensor}, M_{meta}, \Phi, \Lambda)$$

Where:
*   $R \in \text{Tryte}^{81}$: The register file (81 general-purpose registers).
*   $PC \in \mathbb{N}$: The program counter (instruction pointer).
*   $SP \in \mathbb{N}$: The stack pointer.
*   $M_{stack}$: Stack memory segment (LIFO).
*   $M_{heap}$: Heap memory segment (Dynamic).
*   $M_{tensor}$: Tensor storage segment.
*   $M_{meta}$: Meta-programming and reflection segment.
*   $\Phi$: Status flags (Zero, Negative, Positive).
*   $\Lambda$: The Axion audit log (append-only).

> **Implementation**: This state is concretely implemented in `src/vm/vm.cpp` as `struct State`.

### 3.1.2 Transition Function
The transition function $\delta(S_t)$ produces $S_{t+1}$ by executing the instruction at $M_{code}[PC]$.
$$ S_{t+1} = \text{Execute}(\text{Decode}(M_{code}[PC]), S_t) $$

If the Axion Policy Engine denies the transition, the machine transitions to a fault state $\bot$ (Trap).

## 3.2 Memory Layout

The T81VM uses a segmented memory architecture to enforce strict isolation and type safety.

| Segment | Start Index | Role | Access Policy |
| :--- | :--- | :--- | :--- |
| **Code** | 0 | Immutable instructions | Execute-Only (via `Call`), Read-Only (via `MetaRead`) |
| **Stack** | `layout.code.limit` | Function frames | RW (via `Push`/`Pop`) |
| **Heap** | `layout.stack.limit` | Dynamic objects | RW (via Handles) |
| **Tensor** | `layout.heap.limit` | High-dimensional data | RW (via Tensor Opcodes) |
| **Meta** | `layout.tensor.limit` | Reflection data | Read-Only (except via `MetaRefine`) |

> **Source Truth**: Defined in `src/vm/vm.cpp`, `Interpreter` constructor layout initialization.

## 3.3 Register File

The VM exposes 81 registers (`R0`–`R80`).

*   **R0**: Always Zero (Immutable).
*   **R1–R74**: General Purpose.
*   **R75**: Global Tick (Lamport Timestamp).
*   **R76**: Lineage Root Hash.
*   **R77**: Entropy Signature.
*   **R78**: Active Constitutional Mask (Axion).
*   **R79**: Recursion Depth.
*   **R80**: Axion Seal (Halt Status).

> **Verification**: See `sync_system_registers()` in `src/vm/vm.cpp`.

## 3.4 TISC Instruction Set Architecture (ISA)

TISC instructions are fixed-width (81 trits logically, packed into 128-bit or larger structs in C++).

### 3.4.1 Arithmetic Core
*   `Add`, `Sub`, `Mul`, `Div`, `Mod`: Standard integer arithmetic.
*   `Inc`, `Dec`: Increment/Decrement.
*   `Neg`: Negate.

### 3.4.2 Control Flow
*   `Jump`, `JumpIfZero`, `JumpIfNegative`, `JumpIfPositive`.
*   `Call`, `Ret`: Function invocation (pushes `PC` to stack).
*   `Halt`: Stop execution.

### 3.4.3 Memory Access
*   `Load`, `Store`: Register-Memory transfer.
*   `Push`, `Pop`: Stack manipulation.
*   `StackAlloc`, `StackFree`: Frame management.
*   `HeapAlloc`, `HeapFree`: Dynamic memory management.

### 3.4.4 Tensor Operations (Tier 1+)
*   `TNew`, `TSet`, `TGet`: Tensor creation and element access.
*   `TAdd`, `TMul`, `TMatMul`: Vectorized arithmetic.
*   `TRMSNorm`, `TRoPE`, `TSoftmax`: Neural network primitives.

> **Note**: Tensors are opaque handles in the register file (`ValueTag::TensorHandle`). Operations are kernels executed by the host.

### 3.4.5 Axion & Meta Operations
*   `AxRead`, `AxSet`: Policy state access.
*   `MetaRead`, `MetaWrite`: Introspection (Tier 2).
*   `ReflCap`, `ReflTrace`: Execution trace capture.

## 3.5 Fault Semantics

The VM defines precise trap conditions (`Trap` enum in `vm.hpp`):

1.  **DecodeFault**: Invalid opcode or operand.
2.  **StackFault**: Stack overflow or underflow.
3.  **BoundsFault**: Access outside segment limits.
4.  **TypeFault**: Operation on incompatible types (e.g., adding a Tensor to an Int).
5.  **SecurityFault**: Axion Policy denial (e.g., recursion limit).
6.  **DivisionFault**: Division by zero.

Upon a fault, the VM halts immediately, and the `AxionEvent` log records the specific violation with a `Deny` verdict.

## 3.6 Garbage Collection

T81 uses a deterministic **Mark-and-Sweep** collector.
*   **Trigger**: Deterministic instruction count interval (`kGcInterval = 64` instructions).
*   **Roots**: Registers, Stack, and Reflection Snapshots.
*   **Compaction**: The heap is compacted to ensure address stability for subsequent allocations is based on allocation order, not memory fragmentation.

> **Source**: `run_gc_cycle_` in `src/vm/vm.cpp`.

## 3.7 Verification Checklist

*   [ ] **Transition Function**: Does `step()` implement all opcodes in `vm.cpp`?
*   [ ] **Memory Segmentation**: Are `BoundsFault` traps correctly triggered for out-of-segment access?
*   [ ] **Register File**: Are system registers (R75-R80) updated correctly in `sync_system_registers`?
