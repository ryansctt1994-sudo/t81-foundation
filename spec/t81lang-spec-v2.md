---
title: T81Lang Specification (v2.0 - Implementation Accurate)
status: Normative
replaces: T81Lang.pdf
---

# T81Lang Specification (v2.0)

This document serves as the authoritative specification for the T81Lang programming language, as implemented in the current T81 ecosystem. It defines the syntax, semantics, type system, and standard library intrinsics supported by the compiler and runtime.

Unlike previous aspirational documents (e.g., `T81Lang.pdf`), this spec describes **only what is implemented and verifiable** via the repository's deterministic test suite.

## 1. Introduction

T81Lang is a high-level, statically-typed, ternary-native programming language designed for the T81 Virtual Machine (T81VM). It compiles deterministically to Ternary Instruction Set Computer (TISC) bytecode.

### Key Properties
*   **Deterministic:** Compilation and execution are bit-exact reproducible across architectures.
*   **Ternary-Native:** Built on balanced ternary integer (`T81Int`) and floating-point (`T81Float`) primitives.
*   **Pure-by-Default:** Functions are pure unless marked with side-effect intrinsics.
*   **Tier-Aware:** Syntax supports declaring cognitive tiers (`recurse`, `distributed`, `infinite`), though runtime enforcement is currently limited.

## 2. Syntax and Grammar

The following grammar corresponds to the recursive descent parser in `src/frontend/parser.cpp`.

### 2.1. lexical Elements
*   **Comments:** `// line` and `/* block */`.
*   **Identifiers:** Alphanumeric, starting with a letter or underscore. Case-sensitive.
*   **Keywords:** `fn`, `let`, `var`, `if`, `else`, `loop`, `while`, `for`, `match`, `return`, `break`, `continue`, `record`, `enum`, `type`, `recurse`, `distributed`, `infinite`.

### 2.2. Literals
*   **Integer:** `123`, `-42` (Decimal).
*   **Float:** `3.14`, `-0.5`.
*   **Symbol:** `:symbol_name` (Interned unique identifier).
*   **Vector:** `[1, 2, 3]` or `[0; 10]` (Repeat syntax).
*   **Boolean:** `true`, `false` (canonical aliases for `1` and `0` `T81Int`).

### 2.3. Statements

#### Variable Declarations
```t81
let x: T81Int = 42;  // Constant
var y: T81Float = 3.14; // Mutable
```

#### Control Flow
```t81
if (x > 0) { ... } else { ... }

loop {
    if (cond) { break; }
}

@bounded(infinite)
loop { ... } // Explicit infinite loop

while (cond) { ... }

for (i in 0..10) { ... } // Range iteration
```

#### Cognitive Blocks
*Note: These blocks are parsed but currently lower to standard scopes without special opcodes.*
```t81
recurse factorial(n) { ... }
distributed { ... }
infinite { ... }
```

#### Match Expressions
Matches on `Option`, `Result`, and `Enum` types.
```t81
match (opt) {
    Some(x) => x + 1,
    None => 0
}
```

## 3. Type System

### 3.1. Primitive Types
*   `T81Int`: Signed balanced ternary integer.
*   `T81Float`: Floating-point balanced ternary.
*   `T81Fraction`: Rational number.
*   `Symbol`: Interned identifier.
*   `String` / `T81Bytes`: Byte sequences.

### 3.2. Structural Types
*   `Option[T]`: `Some(T)` | `None`.
*   `Result[T, E]`: `Ok(T)` | `Err(E)`.
*   `Record`: Named product type.
    ```t81
    record Point { x: T81Int; y: T81Int; }
    ```
*   `Enum`: Named sum type.
    ```t81
    enum Shape { Circle(T81Int); Square(T81Int); }
    ```

### 3.3. Collections
*   `Vector[T]`: Homogeneous array.
*   `Matrix`, `Tensor`: Multi-dimensional arrays (via intrinsics).

### 3.4. Generics
**Status:** Partial Implementation.
*   **Built-ins:** `Option`, `Result`, `Vector` work as expected.
*   **User-Defined:** Syntax `fn foo[T](...)` is accepted by the parser but **ignored** by the compiler backend. Usage of generic type parameters in user functions is currently **unsafe/unsupported**.

## 4. Standard Library (Intrinsics)

The Standard Library is implemented as compiler intrinsics in `IRGenerator`. These functions map directly to TISC opcodes.

### 4.1. Math (`std.math`)
| Function | TISC Opcode | Description |
| :--- | :--- | :--- |
| `sin(x)`, `cos(x)`, `tan(x)` | `FSIN`, `FCOS`, `FTAN` | Trigonometric functions. |
| `exp(x)`, `log(x)` | `FEXP`, `FLOG` | Exponential and Logarithm. |
| `pow(b, e)` | `FPOW` | Power. |
| `sqrt(x)` | `FSQRT` | Square root. |

### 4.2. Collections (`std.collections`)
| Function | TISC Opcode | Description |
| :--- | :--- | :--- |
| `len(v)` | `VECLEN` | Vector length. |
| `push(v, x)` | `VECPUSH` | Append to vector. |
| `pop(v)` | `VECPOP` | Remove last element. |
| `first(v)`, `last(v)` | `VECFIRST`, `VECLAST` | Access ends. |

### 4.3. Strings & Bytes (`std.text`, `std.bytes`)
| Function | TISC Opcode | Description |
| :--- | :--- | :--- |
| `len(s)` | `STRLEN` | Length. |
| `concat(a, b)` | `STRCONCAT` | Concatenation. |
| `split(s, sep)` | `STRSPLIT` | Split string. |
| `join(parts, sep)` | `STRJOIN` | Join strings. |

### 4.4. System & IO (`std.sys`, `std.io`)
| Function | TISC Opcode | Description |
| :--- | :--- | :--- |
| `print(x)` | `PRINT` | Output to stdout. |
| `exit(code)` | `TRAP` | Halt execution. |
| `time()` | `LOADI` (0) | *Mock implementation (returns 0).* |
| `entropy()` | `LOADI` (0) | *Mock implementation (returns 0).* |

### 4.5. Tensor Operations (`std.tensor`)
| Function | TISC Opcode | Description |
| :--- | :--- | :--- |
| `matmul(a, b)` | `TMATMUL` | Matrix multiplication. |
| `load(path)` | `WEIGHTS_LOAD` | Load tensor from storage. |

## 5. Compilation Pipeline

1.  **Lexing:** Converts source to token stream.
2.  **Parsing:** Produces Abstract Syntax Tree (AST).
3.  **Semantic Analysis:** Resolves names, checks types (partial).
4.  **IR Generation:** Lowering to TISC Intermediate Representation.
    *   Optimizes literals (interning).
    *   Lowers control flow to jumps.
    *   Emits intrinsics as opcodes.
5.  **Code Generation:** Serializes to binary `.tisc` format.

## 6. Determinism & Canonicalization

The compiler guarantees bit-exact output for the same input source.
*   **Vector Literals:** Interned into a deterministic tensor pool.
*   **Symbol Literals:** Mapped to deterministic IDs.
*   **Float Literals:** Parsed using deterministic standard library routines.

## 7. Roadmap & Experimental Features

The following features are parsed but not fully enforced:
1.  **Cognitive Tiers:** `recurse`, `distributed`, `infinite` keywords do not yet trigger VM-level enforcement policies beyond basic recursion limits.
2.  **User Generics:** Fully monomorphized user generics are planned for v2.1.
3.  **AI Optimization:** "Self-reflect" intrinsics exist but the compiler does not perform AI-driven optimization passes.

---
*End of Specification*

## 8. Conformance

This section maps normative requirements to the repository's test suite, ensuring verification.

| Feature | Spec Section | Test Fixtures |
| :--- | :--- | :--- |
| **Control Flow** | §2.3, §6.1-6.3 | `tests/fixtures/t81lang_determinism/07_if_else_print.t81`, `08_bounded_loop_print.t81` |
| **Match Expressions** | §2.3.4 | `tests/fixtures/t81lang_determinism/11_match_option_some_print.t81` |
| **Arithmetic** | §4.1 | `tests/cpp/e2e_arithmetic_test.cpp` |
| **Determinism** | §6 | `tests/fixtures/t81lang_determinism/t81lang_repro_hash.txt` |
| **Vector Literals** | §2.2 | `tests/cpp/frontend_ir_generator_test.cpp` |
