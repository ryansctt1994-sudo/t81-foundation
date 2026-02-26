# Chapter 4: Data Types and Serialization

## 4.1 Primitive Types

**Status: Implemented Core Surfaces**

T81 data types are designed for deterministic representation and canonical behavior.

### 4.1.1 T81Int (Packed Trits)

Integer and trit-oriented representations are foundational to T81's ternary identity.

### 4.1.2 T81Float (Deterministic Float)

Float behavior on deterministic surfaces is governed by project numeric constraints and verification tests. Claims must follow registry/DCP status.

## 4.2 T81Float and dmath

**Status: Implemented with Boundary Controls**

`dmath`-oriented behavior exists to reduce host-dependent math drift on deterministic surfaces.

### 4.2.1 Transcendental Implementation

Transcendental determinism depends on the verified path and test evidence, not narrative assumptions.

## 4.3 Tensors and Canonical Layouts

**Status: Implemented (scope-dependent guarantees)**

Tensor and model workflows are split by maturity:

* DCP deterministic surfaces (where verified),
* governed non-DCP model/inference surfaces,
* experimental research surfaces.

### 4.3.1 Tensor Memory Layout

Canonical layout and stable encoding are prerequisites for reproducibility and content-addressed storage.

### 4.3.2 SafeTensors Integration

SafeTensors and related model formats are integrated via tooling and policy controls. CanonFS/canonical hash workflows are used for governed loading paths.

## 4.4 Canonical Serialization Rules

**Status: Implemented Core Discipline**

Canonical serialization is required so semantically equivalent objects map to stable binary/hash representations.

General rules include:

1. stable primitive encodings,
2. canonical ordering where required,
3. normalized forms for hash-critical artifacts.

Normative definitions live in:

* `spec/t81-data-types.md`
* `spec/cpp-mapping.md`
* `spec/canonfs-spec.md`

<!-- chapter-nav-start -->

---

**Navigation**

- [Book Index](./README.md)
- [Previous: Chapter 3: T81VM Architecture](./03_Architecture.md)
- [Next: Chapter 5: Installation and Build Verification](./05_Installation.md)

<!-- chapter-nav-end -->
