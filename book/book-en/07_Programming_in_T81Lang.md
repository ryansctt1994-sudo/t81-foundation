# Chapter 7: Programming in T81Lang

**Status: Implemented Language Frontend with Ongoing Evolution**

T81Lang is the high-level language for authoring programs that compile into TISC.

This chapter is non-normative. Authoritative language behavior is defined in `spec/t81lang-spec.md` and validated by conformance/regression tests.

## 7.1 Design Philosophy

* Determinism-aware semantics.
* Explicit control flow and type intent.
* Policy-aware runtime boundaries.
* Auditability and reproducibility as core constraints.

## 7.2 Syntax Basics

### 7.2.1 Comments

Line and block comments are supported.

### 7.2.2 Variables

Immutable and mutable bindings are supported with explicit type annotation where needed.

### 7.2.3 Blocks and Scope

Lexical scoping and block expressions are supported within language rules.

## 7.3 Data Types

### 7.3.1 Primitives

Core numeric, boolean, and textual primitives map to VM-compatible representations.

### 7.3.2 Collections

Standard library collection forms are available through language and runtime integration.

### 7.3.3 Enums (Option and Result)

Option/Result-style algebraic modeling is supported in the current language surface.

### 7.3.4 Bitwise Operations

Bitwise operators are available for supported integer-compatible types.

## 7.4 Control Flow

### 7.4.1 Conditionals

Conditional expressions and branching are supported.

### 7.4.2 Pattern Matching

Pattern matching is part of current language coverage and should follow compiler/fixture-tested behavior.

### 7.4.3 Loops

Loop forms are supported and remain subject to runtime policy controls for bounded execution.

## 7.5 Functions

Functions are declared explicitly and compiled into TISC call/return structures.

## 7.6 Structures and Methods

**Status: Active Area**

Structured user types and method-like organization exist, with details defined by the current language spec and compiler behavior.

## 7.7 Axion Integration

Axion policy is a runtime concern. Typical flow:

```bash
t81 check source.t81
t81 compile source.t81 -o program.tisc
t81 run program.tisc --policy policy.apl
```

## 7.8 Examples

### 7.8.1 Hello World

```t81
fn main() -> i32 {
  print("Hello, T81");
  return 0;
}
```

### 7.8.2 Vector Addition

```t81
fn main() -> i32 {
  let a = [1, 2, 3];
  let b = [10, 20, 30];
  print(a[0] + b[0]);
  return 0;
}
```

<!-- chapter-nav-start -->

---

**Navigation**

- [Book Index](./README.md)
- [Previous: Chapter 6: CLI and API Usage](./06_Usage.md)
- [Next: Chapter 8: Verification and Audit](./08_Verification_and_Audit.md)

<!-- chapter-nav-end -->
