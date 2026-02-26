# Chapter 3: T81VM Architecture

## 3.1 Overview

**Status: Current Architecture Narrative**

T81 architecture is organized around deterministic execution controls, policy mediation, and canonical data boundaries.

### 3.1.1 The Execution Pipeline

High-level flow:

1. T81Lang source is parsed and lowered to TISC.
2. TISC executes in the VM interpreter path.
3. Axion policy checks mediate sensitive operations.
4. Trace/audit surfaces record execution evidence.

## 3.2 The Runtime Boundary

**Status: Implemented**

Runtime boundary intent:

* Inputs are explicit: program, policy, and supplied artifacts.
* Outputs are explicit: result, traps, and trace surfaces.
* Side effects are policy-gated.

Normative references:

* `spec/t81vm-spec.md`
* `spec/tisc-spec.md`
* `spec/axion-kernel.md`

## 3.3 Memory Model

**Status: Implemented (with evolving internals)**

The VM uses controlled state and segmented responsibilities to preserve deterministic behavior and safety constraints.

### 3.3.1 Formal State Definition

The machine state includes program counter, stack/register state, memory segments, and policy-visible context.

### 3.3.2 Memory Segments

Conceptual segments include code, stack, heap/object state, and metadata/tracing surfaces, with controlled transitions.

### 3.3.3 Handles and Indirection

Opaque handles are used to avoid exposing host-memory identity as semantic input.

## 3.4 The Instruction Set (TISC)

**Status: Frozen Boundary for DCP-Certified Scope**

TISC is the execution contract between compiler and VM.

### 3.4.1 The Instruction Cycle

Typical cycle: fetch -> decode -> policy check -> execute -> record/advance.

### 3.4.2 Opcode Categories

Core categories include arithmetic, control flow, data movement, and policy-sensitive operations. Exact semantics are in the ISA specification.

## 3.5 JIT Compilation (Trace-JIT)

**Status: Experimental Relative to Determinism Guarantees**

JIT-related runtime paths may exist for optimization research, but deterministic release guarantees remain anchored to explicitly verified surfaces.

### 3.5.1 The Tracing Process

Trace capture can be used for optimization and forensic analysis.

### 3.5.2 Behavioral Equivalence

JIT equivalence must be demonstrated and promoted through governance before deterministic claim expansion.
