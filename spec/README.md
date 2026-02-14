# T81 Specification

> **Source of Truth:** This directory contains the **normative specifications** for the T81 ecosystem.

**Last Updated:** February 10, 2026

## 1. Normative Language

The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**, **MAY**, and **OPTIONAL** in these documents are to be interpreted as described in [RFC 2119](https://tools.ietf.org/html/rfc2119).

## 2. Specification Index

### Core Architecture
- **[t81-overview.md](t81-overview.md):** High-level architectural overview.
- **[t81-data-types.md](t81-data-types.md):** Canonical data types and ternary representation.
- **[cognitive-tiers.md](cognitive-tiers.md):** Definition of cognitive tiers (Tier 1-4).
- **[constitution.md](constitution.md):** Foundational principles and invariants.

### Languages & Formats
- **[t81lang-spec.md](t81lang-spec.md):** T81Lang syntax and semantics.
- **[tisc-spec.md](tisc-spec.md):** Ternary Instruction Set Architecture (ISA).
- **[t81vm-spec.md](t81vm-spec.md):** HanoiVM execution model.
- **[cpp-mapping.md](cpp-mapping.md):** Language bindings for C++.

### System Components
- **[axion-kernel.md](axion-kernel.md):** Axion safety kernel and policy enforcement.
- **[axion-policy-grammar.md](axion-policy-grammar.md):** Grammar for defining Axion policies.
- **[hanoi-kernel-spec.md](hanoi-kernel-spec.md):** Hanoi kernel (OS-level) specification.
- **[canonfs-spec.md](canonfs-spec.md):** CanonFS deterministic filesystem.

## 3. RFC Process (Change Management)

Significant changes to the specification must follow the RFC (Request for Comments) process.
See `rfcs/README.md` for details.

## 4. Historical Context

- **[spec/](spec/):** Contains historical or non-normative companion documents (e.g., `t81-spec.md`).

## 5. Versioning

Specifications are versioned independently of the runtime implementation, but typically align on MAJOR/MINOR numbers. See [VERSIONING.md](../../VERSIONING.md) for the compatibility contract.
