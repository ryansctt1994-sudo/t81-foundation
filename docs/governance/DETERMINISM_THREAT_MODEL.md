# Determinism Threat Model

## 1. Purpose

This document defines the threat landscape for T81's determinism guarantees. It identifies potential failure modes, adversarial vectors, and the mitigation strategies employed to ensure bit-exact reproducibility.

**References:**

* `DETERMINISM_SURFACE_REGISTRY.md` (Defines verified surfaces)
* `FREEZE_ENFORCEMENT.md` (Governs immutable components)
* `SPEC_AUTHORITY_MODEL.md` (Establishes source of truth)
* `INCIDENT_RESPONSE.md` (Defines incident handling protocol)

**Clarification:**
Determinism guarantees apply exclusively to surfaces explicitly listed as **Verified** in the [Determinism Surface Registry](DETERMINISM_SURFACE_REGISTRY.md).

---

## 2. Threat Model Scope

### Protected Assets
This model protects the integrity and reproducibility of:
* **Bytecode Emission:** T81Lang compilation output.
* **Execution Traces:** Instruction-level execution logs.
* **Hash Outputs:** Cryptographic signatures of state and data.

### Adversary Classes
The following threat actors and scenarios are within scope:
* **Accidental Developer Error:** Unintentional introduction of nondeterministic code (e.g., hash map iteration).
* **Compiler/Toolchain Drift:** Variations in binary generation across compiler versions or flags.
* **Cross-Platform Inconsistencies:** Divergence between x86-64 and ARM64 execution.
* **Malicious Contributor:** Intentional injection of subtle nondeterminism or backdoors.
* **Supply Chain Tampering:** Compromise of build dependencies or artifacts.

### Out of Scope
The following are excluded from this specific threat model:
* **Physical Hardware Attacks:** Rowhammer, voltage glitching, or side-channel power analysis.
* **Kernel-Level OS Compromise:** Malicious operating system behavior.
* **Network Timing Nondeterminism:** Variations in packet arrival or latency (unless explicitly modeled).

---

## 3. Threat Categories

### A. Compiler-Level Threats

**Risks:**
* **Undefined Behavior (UB):** Reliance on C++ behaviors not guaranteed by the standard.
* **Compiler Optimization Drift:** Different optimization levels (`-O2` vs `-O3`) altering logic.
* **Standard Library Variance:** Differences in `libc` or `libstdc++` implementations (e.g., `std::sin`).
* **Fast-Math Flags:** Aggressive floating-point optimizations breaking IEEE 754 compliance.
* **ABI Drift:** Binary incompatibility between linked components.

**Mitigations:**
* **Soft-Float Usage:** Mandated use of `T81Float` for all deterministic arithmetic.
* **Repro Gate:** CI jobs enforcing bit-exact binary reproduction.
* **Cross-Arch CI:** Mandatory testing on both x86-64 and ARM64 runners.

---

### B. VM-Level Threats

**Risks:**
* **Memory Allocation Order:** Pointer addresses leaking into logic or output.
* **Hash-Map Iteration:** Unordered containers yielding different traversal sequences.
* **Uninitialized Memory:** Reading garbage values from stack or heap.
* **Time-Dependent Behavior:** Logic branching based on `std::time` or performance counters.
* **Thread Scheduling:** Race conditions or nondeterministic interleaving.

**Mitigations:**
* **Deterministic Data Structures:** Use of ordered maps/sets or stable sorting before iteration.
* **No Wall-Clock Dependency:** Execution logic is decoupled from real time.
* **Controlled Execution Model:** Single-threaded or strictly synchronized execution for verified surfaces.

---

### C. Spec Drift Threats

**Risks:**
* **Spec Updated Without Impl:** Implementation lags behind spec, creating a phantom standard.
* **Impl Updated Without Spec:** Code behavior changes without specification update (de facto standard).
* **Silent Semantic Change:** Subtle alteration of opcode behavior without version bump.

**Mitigations:**
* **Freeze Enforcement:** CI checks preventing modification of frozen directories.
* **Authority Model:** Strict hierarchy placing `/spec` above implementation.
* **Implementation Matrix:** Governance tracking of spec compliance.

---

### D. CI / Governance Bypass Threats

**Risks:**
* **Determinism Gate Disabled:** malicious or accidental removal of CI checks.
* **Artifact Upload Manipulation:** Uploading forged artifacts that match expected hashes.
* **Selective Platform Testing:** Skipping one architecture to hide divergence.

**Mitigations:**
* **Required Status Checks:** Branch protection rules requiring all gates to pass.
* **Cross-Platform Enforcement:** Workflow topology mandates multi-arch success.
* **Public Transparency:** Build logs and artifact hashes are public.

---

### E. Artifact Integrity Threats

**Risks:**
* **CanonFS Hash Manipulation:** falsifying file system metadata.
* **Build Artifact Substitution:** replacing a verified binary with a compromised one.
* **Non-Reproducible Builds:** Builds that cannot be independently verified.

**Mitigations:**
* **Hash Verification:** Cryptographic checking of all inputs and outputs.
* **Reproducibility Gates:** Automated rebuilding of artifacts to verify identity.
* **Controlled Build Process:** Hermetic build environments.

---

### F. Governed AGI Control Threats

**Risks:**
* **Policy Bypass Paths:** AGI-facing logic executes without expected Axion policy checks.
* **Nondeterministic Decision Leakage:** AGI behavior depends on unverified nondeterministic surfaces while presented as deterministic.
* **Unsafe Autonomy Escalation:** Experimental cognitive-tier behavior is exposed without explicit governance boundary classification.
* **Audit Blind Spots:** Missing trace metadata prevents post-incident reconstruction of AGI-related actions.

**Mitigations:**
* **Boundary Classification:** Every AGI-facing release change is classified as DCP, governed non-DCP, or experimental.
* **Governed Promotion Pipeline:** AGI-oriented surfaces must pass ADR, threat-model, registry, and incident-readiness gates before guarantee expansion.
* **Traceability Controls:** Policy/segment/guard metadata tests must remain green for language-to-Axion paths.
* **Incident Trigger Signals:** Severity escalation when policy bypass or deterministic-boundary misrepresentation is detected.

---

## 4. Known Residual Risks

The following risks are acknowledged but not fully mitigated:

* **Partial Compiler Reproducibility:** Full binary bit-exactness across different compiler *vendors* (Clang vs GCC) is an ongoing challenge.
* **Experimental JIT:** The Trace-JIT system is currently experimental and may exhibit subtle divergences.
* **Distributed Tiers:** Consensus mechanisms for distributed execution are not yet verified surfaces.
* **External Model Formats:** Importing third-party models (SafeTensors/GGUF) relies on external parsers which may have edge cases.

---

## 5. Determinism Breach Classification

Breaches of determinism are classified by severity and required response.

| Level | Severity | Impact | Required Action |
| :--- | :--- | :--- | :--- |
| **0** | **Negligible** | No impact on verified surfaces. | Standard bug fix process. |
| **1** | **Minor** | Partial regression in fixture generation or tooling. | Fix bug, add regression test, patch release. |
| **2** | **Critical** | **Verified surface regression.** Output differs across architectures. | **Immediate Revert.** Root cause analysis. Full audit. |
| **3** | **Catastrophic** | Spec-level determinism violation (e.g., ISA definition flaw). | **Major Version Invalidation.** Community notification. |

---

## 6. Review and Audit Process

* **Release Review:** This threat model must be reviewed and approved before any Major or Minor release.
* **New Surfaces:** Any addition to `DETERMINISM_SURFACE_REGISTRY.md` triggers a requirement to update this threat analysis.
* **Breaking Changes:** Any breaking change proposal (RFC) must include an impact analysis referencing this document.
* **Incident Linkage:** Severity 2/3 determinism incidents must be handled under `docs/governance/INCIDENT_RESPONSE.md`.
* **Governed AGI Linkage:** AGI-facing surface promotions must reference
  `docs/status/GOVERNED_AGI_PROMOTION_PIPELINE.md` and include explicit risk
  treatment updates in this document.
