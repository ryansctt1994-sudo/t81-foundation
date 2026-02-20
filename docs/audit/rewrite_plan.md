# Rewrite Plan for T81Lang Specification

This plan outlines the structure and content of the new `spec/t81lang-spec-v2.md`, which will serve as the implementation-accurate normative specification.

## 1. Document Goals
- **Accuracy:** Reflect the current codebase (`parser.cpp`, `ir_generator.hpp`) exactly.
- **Completeness:** Document all supported intrinsics (Standard Library) which were previously implicit or scattered.
- **Clarity:** Distinctly mark features as "Implemented", "Experimental", or "Reserved/Planned".

## 2. Proposed Structure

### I. Introduction
- **Scope:** High-level ternary language compiling to TISC.
- **Key Properties:** Determinism, Purity-by-default, Ternary-native.
- **Toolchain:** `t81 compile` (Lexer -> Parser -> Semantic -> IR -> TISC).

### II. Syntax and Grammar
- **Core Elements:** Comments, Identifiers, Keywords.
- **Literals:** Integers, Floats, Fractions, Symbols (`:sym`), Vectors (`[...]`).
- **Statements:** `let`, `var`, `if`, `loop` (`@bounded`), `match`.
- **Blocks:** `recurse`, `distributed`, `infinite` (defined as semantic blocks, currently lowering to transparent scopes).

### III. Type System
- **Primitive Types:** `T81Int`, `T81Float`, `T81Fraction`, `Symbol`.
- **Composite Types:** `Vector[T]`, `Matrix`, `Tensor`.
- **Structural Types:** `Option[T]`, `Result[T, E]`, `Record`, `Enum`.
- **Generics:** Explicitly state that `Option`, `Result`, and `Vector` are supported built-ins. User-defined generic functions are **Experimental/Unchecked**.

### IV. Semantics and Control Flow
- **Expression Evaluation:** Order of operations, purity.
- **Pattern Matching:** `match` semantics for Option/Result/Enum (exhaustiveness, binding).
- **Loops:** Infinite loops require `@bounded(infinite)`.
- **Recursion:** VM-enforced depth limits.

### V. Standard Library (Intrinsics)
*This section is new and critical, documenting the hardcoded intrinsics.*
- **Math:** `sin`, `cos`, `tan`, `exp`, `log`, `pow`, `sqrt`.
- **String/Bytes:** `len`, `concat`, `split`, `join`, `contains`.
- **Collections:** `len`, `push`, `pop`, `map_get`, `map_put`.
- **System:** `print`, `assert`, `panic`, `time`, `entropy`.
- **AI/Agents:** `self_reflect`.

### VI. Compiler Pipeline & Determinism
- **Stages:** Lexing -> Parsing -> Analysis -> Lowering.
- **Determinism:** CI gates, bit-exact reproduction requirements.

### VII. Cognitive Tiers (Future)
- explain the *intent* of `Tier 1` (Symbolic) to `Tier 5` (Infinite).
- Document current status: Syntax exists, VM enforcement is partial (recursion limit).

### VIII. Appendix: Roadmap & Deprecations
- **Planned:** User-defined generics, Proper `Distributed` opcode support.
- **Deprecated:** "Auto-GPU", "AI-Compiler" magic (replaced by explicit intrinsics).
