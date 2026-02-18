# T81Lang Standard Library Plan

This document outlines the proposed standard libraries for T81Lang, designed to expose the powerful ternary-native types implemented in the `t81-foundation` C++ codebase.

## 1. Design Philosophy

The T81Lang Standard Library adheres to the same core principles as the underlying C++ foundation:

1.  **Strict Determinism**: All library functions must produce bit-identical results across all platforms. Nondeterministic operations (e.g., system time, network IO) are wrapped in deterministic interfaces or explicitly flagged. **Exception:** Floating-point division and transcendentals currently rely on host precision.
2.  **Canonical Representation**: Data structures automatically maintain canonical forms (e.g., normalized fractions, sorted maps) to ensure consistent hashing and serialization.
3.  **Axion Integration**: All resource-intensive or side-effecting operations (IO, memory allocation, tensor ops) must emit Axion trace events and respect Axion policy limits.
4.  **Ternary Native**: Mathematical primitives prioritize balanced ternary arithmetic (`T81Int`, `T81BigInt`, `T81Float`) over binary approximations.

## 2. Library Structure

The standard library is organized into modules under the `std` namespace.

### 2.1 Core & Primitives (`std.core`)
Fundamental types that are likely built-in or implicitly available.

| T81Lang Type | C++ Implementation | Description |
| :--- | :--- | :--- |
| `Int` | `T81Int`, `T81Uint` | Standard balanced ternary integer (typically 27-trit). |
| `BigInt` | `T81BigInt` | Arbitrary-precision integer. |
| `Float` | `T81Float` | Deterministic floating-point number. |
| `Byte` | `T81Byte` | 5-trit byte representation. |
| `Bool` | `bool` (Trit) | Ternary logic values (False, Unknown, True). |
| `Result<T, E>` | `T81Result` | Error handling primitive (no exceptions). |
| `Option<T>` | `Option`, `T81Maybe` | Nullable value wrapper. |

### 2.2 Mathematics (`std.math`)
Extensive mathematical capabilities leveraging the `T81BigInt` and `T81Float` backends.

| Module | C++ Implementation | Features |
| :--- | :--- | :--- |
| `std.math.fraction` | `T81Fraction` | Exact rational arithmetic. |
| `std.math.complex` | `T81Complex` | Complex numbers with ternary components. |
| `std.math.quat` | `T81Quaternion` | Quaternions for 3D rotation and physics. |
| `std.math.poly` | `T81Polynomial` | Polynomial arithmetic and evaluation. |
| `std.math.prob` | `T81Prob` | Probability handling, log-odds arithmetic. |
| `std.math.limb` | `T81Limb` | Low-level multiprecision limb access (advanced). |

### 2.3 Collections (`std.collections`)
Deterministic container types.

| Module | C++ Implementation | Features |
| :--- | :--- | :--- |
| `std.collections.list` | `T81List` | Dynamic array/list implementation. |
| `std.collections.map` | `T81Map` | Ordered map (likely B-Tree or similar). |
| `std.collections.set` | `T81Set` | Ordered set. |
| `std.collections.tree` | `T81Tree` | Generic tree structure for hierarchical data. |
| `std.collections.graph` | `T81Graph` | Graph data structure with node/edge attributes. |

### 2.4 Text & Data (`std.text`, `std.bytes`)
String and binary data manipulation.

| Module | C++ Implementation | Features |
| :--- | :--- | :--- |
| `std.text` | `T81String` | Unicode-aware string handling. |
| `std.bytes` | `T81Bytes` | Raw binary data manipulation. |
| `std.symbol` | `T81Symbol` | Interned strings/atoms for efficient comparison. |

### 2.5 System & Runtime (`std.sys`)
Interaction with the HanoiVM and Axion environment.

| Module | C++ Implementation | Features |
| :--- | :--- | :--- |
| `std.sys.time` | `T81Time` | Deterministic logical time or synchronized real-time. |
| `std.sys.entropy` | `T81Entropy`, `T81Random` | CSPRNG seeded deterministically by the VM. |
| `std.sys.reflect` | `T81Reflection` | Introspection of code and state (Tier 4 capability). |
| `std.sys.proof` | `T81Proof` | Cryptographic proofs of execution/state. |

### 2.6 Input/Output (`std.io`)
Policy-gated I/O operations.

| Module | C++ Implementation | Features |
| :--- | :--- | :--- |
| `std.io.stream` | `T81IOStream`, `T81Stream` | Buffered I/O streams. |
| `std.io.net` | `T81Network`, `T81Discovery` | Network sockets and peer discovery (Axion-gated). |

### 2.7 Concurrency (`std.async`)
Deterministic concurrency primitives.

| Module | C++ Implementation | Features |
| :--- | :--- | :--- |
| `std.async.thread` | `T81Thread` | Green threads or deterministic fibers. |
| `std.async.promise` | `T81Promise` | Futures/Promises for asynchronous results. |

### 2.8 AI & Tensor (`std.tensor`)
High-performance tensor operations for AI workloads.

| Module | C++ Implementation | Features |
| :--- | :--- | :--- |
| `std.tensor` | `T81Tensor` | N-dimensional arrays, autograd support. |
| `std.tensor.dist` | `DistributedTensor` | Tensors spanned across multiple nodes. |
| `std.tensor.matrix` | `T81Matrix` | Specialized 2D matrix operations. |
| `std.tensor.vector` | `T81Vector` | Specialized 1D vector operations. |

### 2.9 High-Level Agents (`std.agent`)
Building blocks for autonomous agents.

| Module | C++ Implementation | Features |
| :--- | :--- | :--- |
| `std.agent` | `T81Agent` | Base class for agents, handling identity and lifecycle. |

## 3. Implementation Strategy

1.  **Native Bindings**: Create a T81Lang binding layer that exposes the C++ `T81*` classes to the interpreter/VM.
2.  **Wrapper Modules**: Write `.t81` source files that provide idiomatic interfaces to these native bindings.
3.  **Axion Hooks**: Ensure every native call injects appropriate Axion trace events (`Axion::emit(...)`) before execution.
4.  **Unit Tests**: Port existing C++ unit tests to T81Lang to verify correctness of the bindings.

## 4. Future Considerations

-   **`std.crypto`**: Cryptographic primitives (hashing, signing) - potentially leveraging `T81Proof`.
-   **`std.physics`**: Engines using `T81Quaternion` and `T81Vector`.
-   **`std.ui`**: Abstract UI definitions for terminal or graphical output.

## 5. Implementation Status (Current)

- `std.math`: `sin`, `cos`, `tan` are wired as deterministic frontend/runtime aliases.
- `std.io`: `println`, `print_int`, and `print_float` aliases lower to `print`.
- `std.tensor`: `load`, `from_list`, `matmul`, and `vec_add` aliases are implemented and tested.
- `std.text`: `str_len`, `str_is_empty`, `concat`, `starts_with`, `ends_with`, `contains`, `index_of`, and `replace` are implemented end-to-end (semantic, IR, VM) and exposed via `src/lang/std/text.t81`.
- `std.text`: fixture-driven CLI golden coverage now validates deterministic runtime output via `tests/fixtures/t81lang_std_text/*` and `tests/cpp/cli_std_text_fixtures_test.cpp`.
- `std.bytes`: `len`, `is_empty`, `concat`, `starts_with`, `ends_with`, `contains`, `index_of`, and `replace` are implemented for frontend-native `T81Bytes`, with explicit `T81Bytes(...)` conversion calls and fixture-driven CLI golden coverage in `tests/fixtures/t81lang_std_bytes/*` and `tests/cpp/cli_std_bytes_fixtures_test.cpp`.
