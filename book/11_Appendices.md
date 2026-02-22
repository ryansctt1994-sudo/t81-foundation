# Chapter 11: Appendices

## 11.1 What Is Not Yet Implemented

**Status: Tracking**

While the core T81VM and TISC ISA are stable, several advanced features remain in active development or research phases.

### 11.1.1 Phase 2: Full `dmath` Coverage
Currently, `dmath` provides deterministic implementations for basic arithmetic (`+`, `-`, `*`, `/`) and key transcendentals (`sin`, `cos`, `exp`, `log`).
*   **Missing**: Inverse trigonometric functions (`asin`, `acos`, `atan`) currently rely on host `libc` (unless `T81_DETERMINISTIC` is set, which traps).
*   **Missing**: Hyperbolic functions (`sinh`, `cosh`, `tanh`) are partial.
*   **Plan**: Implement full Taylor/Maclaurin series expansions for all standard math functions in `include/t81/core/detail/dmath.hpp` to remove all `libm` dependencies.

### 11.1.2 Phase 3: Distributed Consensus (Tier 4)
Tier 4 opcodes (`Gossip`, `Merge`) are specified but the underlying P2P networking stack is experimental.
*   **Missing**: Robust peer discovery (DHT).
*   **Missing**: Sybil resistance mechanism (Proof of Work/Stake placeholder).
*   **Plan**: Integrate a content-addressable networking layer (e.g., libp2p or custom Kademlia) to support decentralized state merging.

### 11.1.3 Phase 4: Full Infinite Forms (Tier 5)
Tier 5 supports basic Geometric Series collapse.
*   **Missing**: General analytic continuation for non-geometric series.
*   **Missing**: Symbolic summation of more complex generating functions.
*   **Plan**: Expand `InfCollapse` to handle a wider class of meromorphic functions.

## 11.2 Glossary

| Term | Definition |
| :--- | :--- |
| **Axion** | The safety kernel of T81, responsible for policy enforcement and audit logging. |
| **CanonRef** | A canonical reference (SHA3-256 hash) pointing to an immutable object in CanonFS. |
| **Cognitive Tier** | A level of computational capability (1=Symbolic to 5=Infinite). |
| **Determinism Gate** | The CI process (`t81lang_repro_gate`) that verifies bit-exact reproducibility of the compiler. |
| **dmath** | Deterministic Math library; a software implementation of floating-point arithmetic. |
| **T81Float** | A balanced ternary floating-point number $(s, m, e)$. |
| **T81Int** | An arbitrary-precision balanced ternary integer. |
| **TISC** | Ternary Instruction Set Computer; the bytecode language of the T81VM. |
| **Trit** | A base-3 digit $\{-1, 0, 1\}$. |
| **Tryte** | A sequence of trits (usually 4). |
| **Structural Honesty** | The principle that a system must not synthesize information or hide approximation. |

## 11.3 Useful Links

*   **Repository**: [github.com/t81-foundation/t81](https://github.com/t81-foundation/t81)
*   **Specification**: `spec/` directory in the repo.
*   **CI Dashboard**: GitHub Actions tab.
