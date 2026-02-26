# Chapter 12: Formal Semantics of TISC and T81VM

## 12.1 Operational Semantics

**Status: Theoretical and Partially Mechanized by Tests**

T81 operational semantics are described by state-transition rules and constrained by VM/ISA normative specs.

### 12.1.1 Transition Rules

Instruction semantics are modeled as small-step transitions over machine state.

## 12.2 Algebraic Transition Function

**Status: Implemented Reference Path**

The interpreter realizes a deterministic transition function for verified surfaces:

$$
S_{t+1} = \delta(S_t, op_t)
$$

## 12.3 Canonicalization Rewrite System

**Status: Implemented Core Discipline**

Canonicalization ensures semantically equivalent objects collapse to stable encoded forms required for deterministic hashing and CanonFS integrity.

## 12.4 Determinism Proof Sketches

### 12.4.1 Floating Point Independence

Proof obligations are tied to deterministic numeric path definitions and validation evidence.

### 12.4.2 Trace Integrity

Trace integrity arguments depend on canonical sequencing and cryptographic hash-chain properties.

## 12.5 Interpreter vs. Trace-JIT Equivalence

**Status: Ongoing Verification Work**

JIT equivalence is not assumed globally; it must be proven for scoped surfaces before deterministic guarantee expansion.

Registry and governance state determine claim strength.

<!-- chapter-nav-start -->

---

**Navigation**

- [Book Index](./README.md)
- [Previous: Chapter 11: Appendices](./11_Appendices.md)
- [Next: Chapter 13: Adversarial Modeling and Determinism Attacks](./13_Adversarial_Modeling.md)

<!-- chapter-nav-end -->
