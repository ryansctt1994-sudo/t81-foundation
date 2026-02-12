# T81 Specification

> **Source of Truth:** This directory contains the **normative specifications** for the T81 ecosystem.

**Last Updated:** February 10, 2026

## 1. Normative Language

The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**, **MAY**, and **OPTIONAL** in these documents are to be interpreted as described in [RFC 2119](https://tools.ietf.org/html/rfc2119).

## 2. Specification Index

- **[lang/](lang/):** T81Lang syntax, type system, and semantics.
- **[tisc/](tisc/):** Ternary Instruction Set Architecture (ISA) and binary format.
- **[vm/](vm/):** HanoiVM execution model, memory layout, and trap behavior.
- **[axion/](axion/):** Policy enforcement engine and trace format.
- **[canonfs/](canonfs/):** Deterministic filesystem interface.
- **[numerics/](numerics/):** Balanced ternary arithmetic (BigInt, Float, Prob, etc.).

## 3. RFC Process (Change Management)

Significant changes to the specification must follow the RFC (Request for Comments) process.

### Lifecycle
1.  **Draft:** A new proposal is created in `spec/rfcs/` using the [template](rfcs/0000-template.md).
2.  **Review:** Community and maintainers discuss the proposal.
3.  **Accepted:** The proposal is approved for implementation.
4.  **Implemented:** The spec changes are merged into the normative documents.
5.  **Rejected:** The proposal is declined.
6.  **Superseded:** A newer RFC replaces this one.

### Supersession
When an RFC is superseded:
- The old RFC is marked as `Status: Superseded` with a link to the new RFC.
- Implementation must be updated to match the new RFC.
- Deprecation policy (see `VERSIONING.md`) applies if existing features are removed.

## 4. Versioning

Specifications are versioned independently of the runtime implementation, but typically align on MAJOR/MINOR numbers. See [VERSIONING.md](../VERSIONING.md) for the compatibility contract.
