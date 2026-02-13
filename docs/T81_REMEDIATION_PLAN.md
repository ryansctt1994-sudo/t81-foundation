# T81 Core Type System Remediation Plan

**Status:** DRAFT
**Target:** T81 Core Architecture
**Scope:** Determinism, Memory Safety, and Canonical Serialization

---

## 1. Determinism Classification Normalization

| Type | Classification | Notes |
| :--- | :--- | :--- |
| `T81Int<N>` | **Strict** | Bitwise-exact behavior across all platforms. |
| `T81Float<M,E>` | **Host-Dependent** | Relies on `double` for division and transcendentals (`sin`, `cos`, `log`). Implementation uses `std::` math functions which may vary by platform/compiler. |
| `T81BigInt` | **Strict** | Implements Knuth's Algorithm D and Karatsuba in software; no host float dependency. |
| `T81Fraction` | **Strict** | Canonicalizes via GCD; operations are integer-based. |
| `T81Complex` | **Host-Dependent** | Inherits `T81Float` dependencies. |
| `T81Quaternion` | **Host-Dependent** | Inherits `T81Float` dependencies. |
| `T81Vector<N>` | **Host-Dependent** | `length()` and `angle()` use `std::sqrt`, `std::acos` via `double`. |
| `T81Matrix` | **Host-Dependent** | Inherits `T81Float` dependencies if used with float scalar. |
| `T81Tensor` | **Host-Dependent** | Inherits scalar dependencies. Activation functions (`gelu`, `softmax`) use `exp`/`tanh` via host float. |
| `T81Symbol` | **Order-Dependent** | ID assignment depends on interning order. `hash()` and `operator<=>` are ID-based, leaking runtime order into logic. |
| `T81Map` | **Order-Dependent** | Iteration order depends on `T81Symbol` ID (if key is symbol) or `std::hash` (if generic). |
| `T81Set` | **Order-Dependent** | Inherits `T81Map` behavior. |
| `T81Graph` | **Strict** | Structure is deterministic, but `NodeID` overflow behavior is hardware/policy dependent (trap). |
| `T81Time` | **Schedule-Dependent** | `now()` reads host clock. |
| `T81Entropy` | **Order-Dependent** | Global `EntropyPool` is sensitive to thread interleaving. |
| `T81IOStream` | **Order-Dependent** | Global state (`cin`, `cout`) mutable by all threads; output order depends on scheduling. |
| `T81List` | **Strict** | Deterministic if element type is deterministic. |
| `T81String` | **Strict** | Text processing is deterministic. |

**Change Justification:** `T81Float` was previously assumed strict, but code review reveals direct `double` casting for critical operations. `T81Symbol` intern ID generation is a major determinism leak.

---

## 2. Global State Mutation Audit

| Type | Global State Access | Determinism Impact | Thread-Safe? | Required Action |
| :--- | :--- | :--- | :--- | :--- |
| `T81Entropy` | `EntropyPool::seed_` | High (Order-Dependent) | **NO** (Race) | **Isolate behind context object** (Thread-local or passed Context) |
| `T81Time` | `deterministic_override_` | High (Schedule/Order) | **NO** (Race) | **Isolate behind context object** (Clock service) |
| `T81Symbol` | `InternTable` singleton | High (ID generation) | **YES** (Mutex) | **Eliminate global state** (Scoped SymbolTable) or **Document as policy boundary** |
| `T81IOStream` | `cin`, `cout`, `cerr` | High (Interleaving) | **NO** (Race) | **Make thread-safe** (Mutex) or **Isolate** (Actor-model IO) |
| `T81BigInt` | None | None | Yes | No action required |
| `T81Float` | None | None | Yes | No action required |

**Critical Findings:**
- `EntropyPool::acquire` modifies static `seed_` without synchronization -> Data race.
- `T81Time::now` reads static `deterministic_override_` which can be written concurrently -> Data race.
- `T81IOStream::write` modifies `mutable T81List<T81Time> timestamps_` without synchronization -> Data race.

---

## 3. SPEC–IMPLEMENTATION GAP Remediation

| Gap | Root Cause | Correction | Classification |
| :--- | :--- | :--- | :--- |
| **T81Float Double Dependency** | `operator/`, `sin`, `cos` cast to `double`. | Implement software ternary division & CORDIC/series for math. | **Code Fix** (Long-term) / **Policy Restriction** (Short-term) |
| **T81Symbol Nondeterminism** | Intern IDs assigned via global counter. | Use content-addressable IDs (hash of string) or scoped tables. | **Type Redesign** |
| **T81Graph Stack Overflow** | `adj` array is `std::array` of size ~13MB (for 6561 nodes). | Change storage to `std::vector` (Heap) or external buffer. | **Code Fix** |
| **T81Tensor Stack Overflow** | `data` array is C-style array in object. Large tensors explode stack. | Change storage to `std::vector` or `std::unique_ptr`. | **Code Fix** |
| **T81Map Iteration Order** | Linear probing + `std::hash` / `symbol_hash`. | Enforce sorted iteration or use deterministic hash (SipHash) + fixed probing. | **Code Fix** |
| **EntropyPool Race** | Unsynchronized static mutation. | Add `std::mutex` or `std::atomic` CAS loop for seed update. | **Code Fix** |
| **IOStream Race** | Unsynchronized `timestamps_` list. | Add `std::mutex` to `T81IOStream`. | **Code Fix** |

**Notes:**
- `T81Graph` and `T81Tensor` currently claim "hardware-native" (stack/static) behavior but their size (`NodeCount=6561`, `Dim=4096`) makes them unusable on standard thread stacks.
- `T81Float` cannot be fixed immediately without a full soft-float library implementation. Policy restriction: "Do not use `T81Float` division/transcendentals in consensus-critical code".

---

## 4. Canonical Serialization Enforcement Plan

| Type | Canonical Required? | Minimal Form | Blocking Issues |
| :--- | :--- | :--- | :--- |
| `T81Int` | Yes | Base-81 String (e.g. `12A`) | None (Implemented) |
| `T81BigInt` | Yes | Base-81 String | None (Implemented) |
| `T81Float` | Yes | Scientific Base-81 (e.g. `1.2A^B`) | `to_string` relies on `to_double`. Needs native formatter. |
| `T81Symbol` | Yes | String Name | ID is local-only; must serialize as String. |
| `T81Map` | Yes | Sorted Key-Value List | Iteration is unsorted. |
| `T81Graph` | Yes | Adjacency List (Sorted) | None. |
| `T81List` | Yes | List `[e1, e2, ...]` | None. |

**Action Items:**
1. Implement `T81Float::to_canonical_string()` without `double` conversion.
2. Implement `T81Map::serialize()` which sorts keys before writing.
3. Ensure `T81Symbol` serializes as its string name, never its ID.

---

## 5. Memory Domain Consistency Plan

| Type | Claimed Domain | Actual Domain | Inconsistency | Action |
| :--- | :--- | :--- | :--- | :--- |
| `T81Int` | Stack | Stack | None | None |
| `T81Float` | Stack | Stack | None | None |
| `T81BigInt` | Heap | Heap | None | None |
| `T81List` | Dynamic | Heap (`std::vector`) | None | None |
| `T81Map` | Dynamic | Heap (`std::vector`) | None | None |
| **`T81Graph`** | **Hardware/Static** | **Stack (Huge)** | **Stack Overflow Risk** | **Move to Heap (`std::vector`)** |
| **`T81Tensor`** | **Hardware/Static** | **Stack (Huge)** | **Stack Overflow Risk** | **Move to Heap (`std::unique_ptr`)** |
| `T81IOStream` | Static Global | Global | None | None |

**Proposal:**
- Convert `T81Graph`'s `EdgeList adj[NodeCount]` to `std::vector<EdgeList> adj`.
- Convert `T81Tensor`'s `Element data[...]` to `std::unique_ptr<Element[]> data`.
- Add `T81StackTensor` for small, strictly stack-allocated tensors (e.g. `Vec3`, `Mat3x3`).

---

## 6. Determinism Boundary Definition for VM Core

| Type | VM-Safe | Conditions | Required Guards |
| :--- | :--- | :--- | :--- |
| `T81Int` | **YES** | None | None |
| `T81BigInt` | **YES** | None | Memory Quota (Heap) |
| `T81Float` | **CONDITIONAL** | No div/transcendentals | Strict Ops Only / Soft-Float Polyfill |
| `T81List` | **YES** | None | Memory Quota |
| `T81Map` | **NO** | Order-Dependent | Use `T81OrderedMap` or sort before hash/iterate |
| `T81Graph` | **NO** | Stack Overflow | Must use Heap-backed variant |
| `T81Tensor` | **NO** | Stack Overflow / Float dependency | Use Heap-backed variant; No Float Transcendentals |
| `T81Symbol` | **NO** | Order-Dependent ID | Use String-only or Scoped Table |
| `T81Time` | **NO** | Nondeterministic | Inject Time via VM Context |
| `T81Entropy` | **NO** | Side-effecting | Inject Entropy via VM Context |
| `T81IOStream` | **NO** | Side-effecting | Ban. Use VM Output Buffer. |

---

## 7. Minimal Fix Roadmap (Prioritized)

| Priority | Component | Change Type | Scope | Description |
| :--- | :--- | :--- | :--- | :--- |
| **P0** | `T81Entropy` | Code | Small | **Fix Data Race:** Add `std::mutex` to `EntropyPool` or use atomics. |
| **P0** | `T81IOStream` | Code | Small | **Fix Data Race:** Add `std::mutex` to protect `timestamps_` and `fwrite`. |
| **P0** | `T81Time` | Code | Small | **Fix Data Race:** Protect `deterministic_override_` with mutex or atomic. |
| **P1** | `T81Graph` | Code | Medium | **Fix Stack Overflow:** Change `adj` to `std::vector`. |
| **P1** | `T81Tensor` | Code | Medium | **Fix Stack Overflow:** Change `data` to heap allocation for Rank > 1 or Size > 1KB. |
| **P1** | `T81Symbol` | Spec/Code | Medium | **Fix Determinism:** Define canonical ID generation or prohibit logic based on ID order. |
| **P2** | `T81Map` | Code | Medium | **Fix Iteration:** Implement `serialize()` with sorted keys. |
| **P2** | `T81Float` | Code | Large | **Fix Determinism:** Implement `to_string` without `double`. |
| **P3** | `T81Float` | Spec | Large | **Float Math:** Replace `std::sin` etc. with ternary series implementations. |

---
