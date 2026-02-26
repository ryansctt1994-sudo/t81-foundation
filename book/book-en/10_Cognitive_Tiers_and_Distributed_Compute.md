# Chapter 10: Cognitive Tiers and Distributed Compute

## 10.1 The Cognitive Tier Model

**Status: Mixed (core concepts implemented; higher tiers not DCP-certified)**

T81 uses cognitive tiers to separate bounded deterministic execution from higher-risk or research-oriented surfaces.

| Tier | Name | Typical Surface | Release Classification |
| :--- | :--- | :--- | :--- |
| **1** | Symbolic | baseline deterministic execution | DCP candidate/verified surfaces |
| **2** | Reflective | introspection and metadata-aware flows | governed, scope-dependent |
| **3** | Recursive | bounded recursion and advanced control | governed, scope-dependent |
| **4** | Distributed | cross-node merge/consensus experiments | experimental, non-DCP |
| **5** | Infinite | infinite-form or unbounded-form research | experimental, non-DCP |

### 10.1.1 Promotion Mechanism

Promotion is policy-gated. Axion policy and governance state determine whether a surface can move from experimental to stronger assurance states.

## 10.2 Distributed Compute (Tier 4)

**Status: Experimental**

Tier 4 work covers distributed state movement and merge semantics, but deterministic guarantee scope is not equivalent to DCP interpreter/ISA guarantees.

### 10.2.1 Governance Position

Distributed surfaces are explicitly outside DCP unless promoted through registry updates and governance approval.

### 10.2.2 Threat Model Focus

Distributed design must account for replay/ordering attacks, delayed-state publication, and consensus drift. Determinism claims are conditional on explicit verified scope.

## 10.3 Trace-JIT and Advanced Runtime Paths

**Status: Experimental relative to determinism guarantees**

Trace-JIT may exist for performance work, but deterministic release guarantees remain anchored to verified surfaces. JIT equivalence must be explicitly proven and promoted before deterministic claims are expanded.

## 10.4 Infinite Forms (Tier 5)

**Status: Research-oriented**

Tier 5 concerns non-terminating or convergence-oriented models and is treated as a research frontier, not a DCP-stable runtime guarantee.

### 10.4.1 Practical Constraint

Any Tier 5 execution path requires explicit policy controls and should be treated as governed non-DCP behavior unless promoted via the governed AGI pipeline.

### 10.4.2 Promotion Path

Promotion lifecycle: Planned -> Experimental -> Verified Candidate -> Verified, with mandatory evidence gates as documented in `docs/status/GOVERNED_AGI_PROMOTION_PIPELINE.md`.
