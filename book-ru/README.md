# The T81 Foundation — Definitive Technical Monograph

## Foreword

There are two ways to build systems.

One is to optimize for convenience — to move quickly, to approximate, to accept that the final bit may vary, that floating-point drift is tolerable, that compilers may reorder, that hardware will decide what “close enough” means.

The other is to insist that computation is not suggestion, but statement.

T81 belongs to the second path.

At its core, this project is not about ternary arithmetic, virtual machines, or policy engines — though it contains all of these. It is about **integrity of execution**. It is about drawing a boundary around a computational process and saying: inside this boundary, behavior is not incidental.

Determinism is often treated as a performance tradeoff or a debugging convenience. Here it is treated as a civilizational constraint. If two machines cannot agree on the outcome of the same program, then the computation was never truly defined — it was merely performed.

Balanced ternary, canonical serialization, software-defined math, trace logging, policy enforcement — these are not aesthetic choices. They are instruments in a single argument:

> A computation should be reproducible, auditable, and structurally honest.

Modern systems are layered with abstraction that hides state transitions behind optimizers, speculative execution, floating-point quirks, and implicit side effects. T81 attempts something different: to make every transition explicit, every representation canonical, every execution traceable.

It is an architectural experiment in constraint.

The system does not assume benevolent hardware.
It does not assume identical floating-point libraries.
It does not assume compilers behave the same across architectures.
It does not assume that execution without record is acceptable.

Instead, it encodes rules:

* State transitions must be definable.
* Data must have a single canonical form.
* Resource consumption must be accountable.
* Policies must be enforceable.
* Behavior must be replayable.

The result is not the fastest machine.
It is not the most flexible environment.
It is not designed to replace general-purpose scripting ecosystems.

It is designed to answer a narrower but more demanding question:

**Can a software system be constructed such that its behavior is provably invariant across space and time?**

This book exists to document that attempt.

Not as mythology.
Not as marketing.
But as a ledger.

Every subsystem described here — T81Lang, TISC, the T81VM, Axion, CanonFS, the determinism gates, the cognitive tiers — is part of a layered structure built around one invariant:

> Identical inputs must produce identical outputs, under explicitly defined rules.

Whether this architecture becomes widely adopted is secondary. What matters is that it has been made concrete, implemented, tested, and described with enough precision that it can be understood, verified, or challenged by others.

This volume is therefore both technical and philosophical.

It is technical because it describes a working system.
It is philosophical because it asserts that reproducibility is not optional in certain domains.

If the repository evolves, this book should evolve with it.
If the project ceases, this document should remain sufficient to reconstruct what was attempted and why.

In the end, T81 is not a claim of perfection.

It is a commitment to constraint.

And constraint, when applied deliberately, is a form of clarity.

---

## How to Read This Book

* **New to T81?** → Start with Part I, then Part II.
* **Implementer?** → Focus on Parts II and III.
* **Auditor?** → Read Parts III and IV carefully.
* **Researcher?** → Emphasize Parts IV and V.
* **Long-term Maintainer?** → Parts IV and V are critical.

---

## Navigation

<details open>
<summary><strong>Part I — Foundations</strong></summary>

1. **[Introduction](./01_Introduction.md)**

   * [1.1 Scope and Definition](./01_Introduction.md#11-scope-and-definition)
   * [1.2 System Architecture](./01_Introduction.md#12-system-architecture)
   * [1.3 Verifiable Compute Mission](./01_Introduction.md#13-verifiable-compute-mission)

2. **[Core Principles and Invariants](./02_Core_Principles_and_Invariants.md)**

   * [2.1 The Determinism Invariant](./02_Core_Principles_and_Invariants.md#21-the-determinism-invariant)
   * [2.1.1 Determinism Surfaces and Attack Vectors](./02_Core_Principles_and_Invariants.md#211-determinism-surfaces-and-attack-vectors)
   * [2.2 Ternary Logic (Base-3)](./02_Core_Principles_and_Invariants.md#22-ternary-logic-base-3)
   * [2.3 Auditability and the Axion Trace](./02_Core_Principles_and_Invariants.md#23-auditability-and-the-axion-trace)
   * [2.4 The Nine Principles (Ethics Enforcement)](./02_Core_Principles_and_Invariants.md#24-the-nine-principles-ethics-enforcement)

</details>

---

<details>
<summary><strong>Part II — The Deterministic Machine</strong></summary>

3. **[T81VM Architecture](./03_T81VM_Architecture.md)**

   * [3.1 Formal State Machine](./03_T81VM_Architecture.md#31-formal-state-machine)
   * [3.1.1 State Definition](./03_T81VM_Architecture.md#311-state-definition)
   * [3.2 Memory Layout](./03_T81VM_Architecture.md#32-memory-layout)
   * [3.3 Register File](./03_T81VM_Architecture.md#33-register-file)
   * [3.4 TISC Instruction Set Architecture](./03_T81VM_Architecture.md#34-tisc-instruction-set-architecture-isa)
   * [3.5 Fault Semantics](./03_T81VM_Architecture.md#35-fault-semantics)
   * [3.6 Garbage Collection](./03_T81VM_Architecture.md#36-garbage-collection)

4. **[Data Types and Canonical Serialization](./04_Data_Types_and_Canonical_Serialization.md)**

   * [4.1 Primitive Types](./04_Data_Types_and_Canonical_Serialization.md#41-primitive-types)
   * [4.2 T81Float and dmath](./04_Data_Types_and_Canonical_Serialization.md#42-t81float-and-dmath)
   * [4.3 Tensors and Canonical Layouts](./04_Data_Types_and_Canonical_Serialization.md#43-tensors-and-canonical-layouts)
   * [4.4 Canonical Serialization Rules](./04_Data_Types_and_Canonical_Serialization.md#44-canonical-serialization-rules)

5. **[Installation and Build Verification](./05_Installation_and_Build_Verification.md)**

   * [5.1 Prerequisites](./05_Installation_and_Build_Verification.md#51-prerequisites)
   * [5.2 Building from Source](./05_Installation_and_Build_Verification.md#52-building-from-source)
   * [5.3 Verifying the Build](./05_Installation_and_Build_Verification.md#53-verifying-the-build)

6. **[CLI and API Usage](./06_CLI_and_API_Usage.md)**

   * [6.1 Command Line Interface](./06_CLI_and_API_Usage.md#61-the-t81-command-line-interface)
   * [6.2 Embedding T81 (C++ API)](./06_CLI_and_API_Usage.md#62-embedding-t81-c-api)
   * [6.3 Embedding T81 (Python API)](./06_CLI_and_API_Usage.md#63-embedding-t81-python-api)
   * [6.4 Debugging](./06_CLI_and_API_Usage.md#64-debugging)

</details>

---

<details>
<summary><strong>Part III — Governance and Verification</strong></summary>

7. **[Verification and Audit](./07_Verification_and_Audit.md)**

   * [7.1 Formal Verification Methodology](./07_Verification_and_Audit.md#71-formal-verification-methodology)
   * [7.2 The Formal Audit Matrix](./07_Verification_and_Audit.md#72-the-formal-audit-matrix)
   * [7.3 Property-Based Testing](./07_Verification_and_Audit.md#73-property-based-testing)
   * [7.4 The Determinism Gate](./07_Verification_and_Audit.md#74-the-determinism-gate)

8. **[The Axion Safety Kernel](./08_The_Axion_Safety_Kernel.md)**

   * [8.1 Formal Definition](./08_The_Axion_Safety_Kernel.md#81-formal-definition)
   * [8.2 The Policy Model](./08_The_Axion_Safety_Kernel.md#82-the-policy-model)
   * [8.3 Instruction Interception](./08_The_Axion_Safety_Kernel.md#83-instruction-interception)
   * [8.4 The Audit Log (Trace)](./08_The_Axion_Safety_Kernel.md#84-the-audit-log-trace)
   * [8.5 Cognitive Promotion](./08_The_Axion_Safety_Kernel.md#85-cognitive-promotion)

9. **[Cognitive Tiers and Distributed Compute](./09_Cognitive_Tiers_and_Distributed_Compute.md)**

   * [9.1 The Cognitive Tier Model](./09_Cognitive_Tiers_and_Distributed_Compute.md#91-the-cognitive-tier-model)
   * [9.2 Distributed Compute (Tier 4)](./09_Cognitive_Tiers_and_Distributed_Compute.md#92-distributed-compute-tier-4)
   * [9.3 Trace-Based JIT Compilation](./09_Cognitive_Tiers_and_Distributed_Compute.md#93-trace-based-jit-compilation)
   * [9.4 Infinite Forms (Tier 5)](./09_Cognitive_Tiers_and_Distributed_Compute.md#94-infinite-forms-tier-5)

10. **[Appendices](./10_Appendices.md)**

* [10.1 What Is Not Yet Implemented](./10_Appendices.md#101-what-is-not-yet-implemented)
* [10.2 Threat Model and Determinism Attack Surface](./10_Appendices.md#102-threat-model-and-determinism-attack-surface)
* [10.3 Glossary](./10_Appendices.md#103-glossary)

</details>

---

<details>
<summary><strong>Part IV — Formalization and Structural Hardening</strong></summary>

11. **[Formal Semantics of TISC and T81VM](./11_Formal_Semantics.md)**

* [Denotational Semantics of TISC](./11_Formal_Semantics.md#denotational-semantics-of-tisc)
* [Algebraic Transition Function δ](./11_Formal_Semantics.md#algebraic-transition-function-δ)
* [Canonicalization Rewriting System](./11_Formal_Semantics.md#canonicalization-rewriting-system)
* [Determinism Proof Sketches](./11_Formal_Semantics.md#determinism-proof-sketches)
* [Interpreter vs Trace-JIT Equivalence](./11_Formal_Semantics.md#interpreter-vs-trace-jit-equivalence)

12. **[Adversarial Modeling and Determinism Attacks](./12_Adversarial_Modeling.md)**

* [Compiler-Level Attacks](./12_Adversarial_Modeling.md#compiler-level-attacks)
* [VM and GC Attack Vectors](./12_Adversarial_Modeling.md#vm-and-gc-attack-vectors)
* [CanonFS and Hash Attacks](./12_Adversarial_Modeling.md#canonfs-and-hash-attacks)
* [Distributed Tier Time-Travel Attack](./12_Adversarial_Modeling.md#distributed-tier-time-travel-attack)
* [Determinism Breach Postmortem Template](./12_Adversarial_Modeling.md#determinism-breach-postmortem-template)

</details>

---

<details>
<summary><strong>Part V — Continuity and Research Horizon</strong></summary>

13. **[Continuity and Resilience](./13_Continuity_Resilience.md)**

* [Cleanroom Reconstruction Protocol](./13_Continuity_Resilience.md#cleanroom-reconstruction-protocol)
* [Single Points of Failure](./13_Continuity_Resilience.md#single-points-of-failure)
* [Continuity Manifest](./13_Continuity_Resilience.md#continuity-manifest)
* [Immutable Formal Invariants](./13_Continuity_Resilience.md#immutable-formal-invariants)

14. **[Research Frontier](./14_Research_Frontier.md)**

* [Ternary Hardware Acceleration](./14_Research_Frontier.md#ternary-hardware-acceleration)
* [Formal Verification Paths](./14_Research_Frontier.md#formal-verification-paths)
* [CanonFS as a Merkle Substrate](./14_Research_Frontier.md#canonfs-as-a-merkle-substrate)
* [Deterministic AI Inference at Scale](./14_Research_Frontier.md#deterministic-ai-inference-at-scale)

</details>
