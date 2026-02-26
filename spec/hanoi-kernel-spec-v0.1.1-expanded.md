---
title: Hanoi Kernel — Reference Specification (Unified)
version: 0.1.1 (Historical / Experimental)
status: archived
category: kernel
created: 2025-11-22
updated: 2025-11-22
---

> **Note:** This specification describes an experimental microkernel architecture ("Hanoi Kernel") which differs from the current normative TISC and VM specifications. For the authoritative definition of the TISC ISA and HanoiVM execution model, refer to [`tisc-spec.md`](tisc-spec.md) and [`t81vm-spec.md`](t81vm-spec.md).

# Hanoi Kernel v0.1.1

**A Deterministic, Capability-Native, Axion-Governed Microkernel for T81-Class Machines**

This specification defines the complete behavior of the Hanoi kernel:
architecture, boot flow, syscall interface, ABI, deterministic entropy model,
CanonSeal key derivation, diagrams, and implementation scaffold.

This is the canonical v0.1.1 revision.

______________________________________________________________________

# 0. Glossary & Formal Definitions

Before proceeding, the following terms are rigorously defined to prevent ambiguity in this specification.

- **Axion Θ₁–Θ₉**: The nine ethical axioms encoded as immutable constraints within the Axion Co-Processor. These govern all state transitions, prohibiting unsafe recursion, resource exhaustion, and ethical violations.
- **CanonBlock**: The atomic unit of storage in CanonFS. It is exactly 729 trytes in size.
- **CanonRef**: A content-addressed reference to a CanonBlock, consisting of `CanonHash-81(payload)`. It serves as a pointer in the Merkle-81 tree.
- **CanonSeal**: A cryptographic envelope (T81-AEAD-81) that wraps a CanonObject, ensuring confidentiality and integrity. The key is derived deterministically from the object's identity and its capability context.
- **CanonHash-81**: The mandatory cryptographic hash function (based on BLAKE3 projected into Base-81) used for all content addressing.
- **CanonParity**: Reed-Solomon error correction shards stored alongside data to allow deterministic recovery of lost blocks.
- **Merkle-81**: A Merkle tree structure where each node has up to 81 children, optimizing for ternary logic and CanonBlock alignment.
- **TISC (Ternary Instruction Set Computer)**: The underlying instruction set architecture executing on the hardware or simulator.
- **T81VM**: The virtual machine running user-space code, providing the environment for T81Lang execution.
- **HanoiError**: The exhaustive set of possible kernel failures, modeled as a deterministic enum.
- **Tryte**: A unit of information consisting of 3 trits, capable of representing values from -13 to +13 (balanced) or 0 to 26 (unsigned).
- **SnapshotRef**: A handle to a specific, immutable CanonFS root, representing the entire system state at a specific tick.
- **LZ81**: A Lempel-Ziv variant optimized for tryte streams, used for deterministic compression.
- **Z3std**: A Zstandard variant adapted for high-entropy ternary data, used for large tensor compression.
- **DRBG (Deterministic Random Bit/Tryte Generator)**: A mechanism to produce pseudo-randomness solely from a deterministic seed, ensuring perfect replayability.

______________________________________________________________________

# 1. Executive Summary

Hanoi is a **deterministic ternary microkernel** serving as the constitutional OS substrate for the T81 ecosystem:

- TISC instruction execution
- T81VM and T81Lang runtime
- CanonFS v0.4.1 as the only storage
- Axion Θ₁–Θ₉ ethical enforcement
- T243 → T19683 cognitive tier promotion

It is designed to be:

- Fully deterministic
- Capability-native
- Immutable
- Perfectly replayable
- Axion-supervised at every transition
- Self-healing
- Rooted in content addressing and CanonFS snapshots

Hanoi is not a general-purpose OS.\
It is the *execution substrate* for T81-class machines.

______________________________________________________________________

# 2. Kernel Invariants (The Hanoi Creed)

Violation of any invariant means the system is **not Hanoi**.

01. **No mutable global state** outside capability boundaries.
02. **Every object is a CanonRef** (content hash + capability + optional sealing).
03. **CanonFS is the sole storage abstraction**; the root is always a snapshot.
04. **Axion veto authority** extends to all syscalls and state transitions.
05. **Syscalls are total functions** returning `Result<T, HanoiError>`.
06. **Deterministic scheduling** — global 81-slot tick.
07. **No userspace drivers** — all kernel drivers are CanonFS modules.
08. **Boot requires full canonical verification** (CanonHash-81 + CanonParity).
09. **Entropy is deterministic**, seeded from the snapshot; no nondeterministic RNG.
10. **Sealed objects use derived per-object keys**, no mutable key state.

______________________________________________________________________

# 3. High-Level Architecture

```mermaid
graph TD
    subgraph "Userland (Ring 3)"
        T81VM[T81VM + T81Lang Runtime]
    end

    subgraph "Hanoi Kernel (Ring 0)"
        CapMgr[Capability Manager]
        CanonFS[CanonFS Driver v0.4.1]
        Scheduler[Deterministic Scheduler]
        MemMgr[Linear Memory Manager]
        Syscalls[Syscall Layer]
        DRBG[DRBG / Deterministic Time]
    end

    subgraph "Hardware / Simulation"
        Axion[Axion Co-Processor]
        TISC[TISC Execution Engine]
        HW[Ternary Hardware / Hanoi Simulator]
    end

    T81VM <--> Syscalls
    Syscalls --> CapMgr
    Syscalls --> CanonFS
    Syscalls --> Scheduler
    Syscalls --> MemMgr

    CapMgr -.-> Axion
    CanonFS -.-> Axion
    Scheduler -.-> Axion

    CapMgr --> TISC
    CanonFS --> TISC
    Scheduler --> TISC

    Axion --> TISC
    TISC --> HW
```

The architecture strictly separates concerns. The **Hanoi Kernel** mediates all access to hardware resources through the **Axion Co-Processor**, which validates every privileged operation against the system invariants.

______________________________________________________________________

# 4. Boot Process — 7 Deterministic Stages

The boot process is a rigorous chain of trust. Every stage must verify the next before yielding control.

| Stage | Name | Action | Failure → |
|-------|------------------|--------------------------------------------------|-----------|
| 0 | ROM Stub | Load first 729-tryte CanonBlock from boot medium | AXHALT |
| 1 | CanonVerify | Verify snapshot integrity via CanonHash-81 & CanonParity | AXHALT |
| 2 | Decompress | Inflate kernel image (LZ81/Z3std) → RAM | AXHALT |
| 3 | AxionSeal | Load Axion ethics module (Θ₁–Θ₉) and lock policies | AXHALT |
| 4 | CapabilityRoot | Mint root capability from snapshot hash | AXHALT |
| 5 | Mount CanonFS | Mount root snapshot at `/` (e.g., `/snapshot/latest`) | AXHALT |
| 6 | Start T81VM | Exec `CanonExec` binary as PID 1 | AXHALT |
| 7 | Enter Userspace | Begin deterministic global tick (Tick 0) | — |

On any failure:

```
AXHALT(reason):
    freeze state → emit canonical lineage dump → halt permanently
```

### Boot Flow Diagram

```mermaid
sequenceDiagram
    participant ROM as ROM Stub
    participant Verify as CanonVerify
    participant Axion as Axion Kernel
    participant FS as CanonFS
    participant VM as T81VM

    ROM->>Verify: Load Boot Block
    Verify->>Verify: Check CanonHash & Parity
    Verify->>Axion: Load Kernel & Ethics Module
    Axion->>Axion: Initialize Invariants (Θ₁–Θ₉)
    Axion->>FS: Mount Root Snapshot
    FS->>VM: Spawn PID 1 (CanonExec)
    VM->>VM: Begin Execution (Tick 0)
```

______________________________________________________________________

# 5. Core Kernel Subsystems

## 5.1 Capability Manager

### Rationale
Hanoi rejects Discretionary Access Control (DAC) and Access Control Lists (ACLs) in favor of Object Capabilities (OCap). Security is not a check on "who you are" but "what you hold".

### Formal Model
A capability is a tuple:
`Cap := (ObjectRef, PermMask, ScopeID, AxionSig)`
- `ObjectRef`: CanonHash-81 of the target.
- `PermMask`: Bitmask of allowed operations (Read, Write, Exec, Grant).
- `ScopeID`: Topological region of the object graph.
- `AxionSig`: Cryptographic proof of validity.

### Invariants Enforced
1. **Unforgeability**: Capabilities cannot be created by userspace, only granted.
2. **Connectivity**: Access implies possession of a valid capability.
3. **Revocation**: Revocation is immediate via Axion signal propagation.

### Edge Cases & Failure Modes
- **Dangling Capability**: If an object is garbage collected, the capability becomes inert but safe. Access attempts return `CapabilityRevoked`.
- **Scope Violation**: Passing a capability outside its allowed scope triggers an Axion veto.

### Example Usage
```cpp
// Grant read access to a specific block
auto read_cap = kernel.grant_cap(target_block, PERM_READ);
if (!read_cap) return read_cap.error();
process.send(recipient_pid, *read_cap);
```

## 5.2 CanonFS Driver (Ring 0)

### Rationale
Storage is the single source of truth. By making the filesystem a Merkle-81 tree, the entire system state becomes a single hash.

### Formal Model
CanonFS implements a content-addressed store.
`Store: Hash -> CanonBlock`
Writes do not mutate in place; they return a new `SnapshotRef`.

### Invariants Enforced
1. **Immutability**: Blocks are never modified.
2. **Self-Healing**: Read failures trigger automatic reconstruction from parity shards.
3. **Deduplication**: Identical content shares the same storage address.

### Edge Cases & Failure Modes
- **Parity Exhaustion**: If shards < threshold, `read_block` fails with `CanonCorruption`.
- **Hash Collision**: Theoretically impossible with CanonHash-81; treated as catastrophic invariant failure (`AXHALT`).

### Example Usage
```cpp
auto block = canonfs.read_block(file_hash);
// Automatically verified and repaired
```

## 5.3 Axion Co-Processor

### Rationale
Ethics and safety cannot be left to software conventions. They must be enforced by a privileged monitor that supervises execution.

### Formal Model
Axion operates as a reference monitor `M`.
`M(State, Op) -> { Allow, Deny(Reason), Rewrite(NewOp) }`

### Invariants Enforced
1. **Θ₁–Θ₉ Compliance**: No state transition may violate the ethical axioms.
2. **Resource Boundedness**: No operation may exceed its allocated tick budget.

### Edge Cases & Failure Modes
- **Veto**: Axion denies a syscall. The process receives `AxionRejection`.
- **Panic**: If Axion detects an internal inconsistency, it halts the entire kernel (`AXHALT`).

## 5.4 Deterministic Scheduler

### Rationale
Nondeterministic interleaving is the root of most concurrency bugs. Hanoi schedules strictly by tick count.

### Formal Model
Round-robin scheduling over 81 slots.
`Schedule(Tick) -> Pid = ActivePids[Tick % 81]`

### Invariants Enforced
1. **Tick Determinism**: `State(Tick)` is strictly a function of `State(Tick-1)`.
2. **Fairness**: Every active process gets exactly 1 slot per 81 ticks.

### Edge Cases & Failure Modes
- **Budget Exhaustion**: Process halted mid-instruction if tick budget exceeded.
- **Idle**: If no process is runnable, the kernel advances the tick counter (skip).

## 5.5 Memory Manager

### Rationale
Manual memory management is unsafe. Hanoi uses a linear memory model backed by CanonFS for persistence.

### Formal Model
Memory is a sequence of Pages.
`Page := (CanonRef, Permissions)`
Allocations are strictly strictly bump-pointer or COW from CanonFS.

### Invariants Enforced
1. **No Use-After-Free**: Memory is reclaimed only when no capabilities reference it.
2. **Isolation**: Processes cannot address memory outside their capability bounds.

### Example Usage
```cpp
auto region = memory.map_region(canon_ref, PERM_RW);
// region is now accessible in process address space
```

## 5.6 Syscall Layer

### Rationale
The kernel surface must be minimal and total. Every syscall returns a result, never undefined behavior.

### Formal Model
`Syscall: (OpCode, Args...) -> Result<Value, HanoiError>`

______________________________________________________________________

# 6. Snapshot Lifecycle

Snapshots are the heart of Hanoi's state management. A snapshot represents the entire persistent state of the system at a given moment.

### Lifecycle State Machine

```mermaid
stateDiagram-v2
    [*] --> Pending: fork_snapshot()
    Pending --> Committing: Axion Approval
    Pending --> Discarded: Drop / Reject
    Committing --> Active: switch_root()
    Active --> Archived: new switch_root()
    Active --> [*]: System Halt
    Archived --> [*]: GC
```

### 6.1 fork_snapshot()

```cpp
auto fork_snapshot() -> expected<SnapshotRef, HanoiError>
```
Creates a mutable staging area (Copy-On-Write) from the current root. The new root is not yet visible to other processes.
- **Pre-condition**: Caller has `CAP_SNAPSHOT_CREATE`.
- **Post-condition**: Returns a handle to a new, isolated Merkle root.

### 6.2 commit_snapshot(snapshot)

```cpp
auto commit_snapshot(SnapshotRef snapshot) -> expected<void, HanoiError>
```
Finalizes a pending snapshot, calculating the new Merkle root hash and persisting all dirty blocks.
- **Pre-condition**: `snapshot` is valid and owned by caller.
- **Axion Check**: Verifies integrity and policy compliance.
- **Post-condition**: Snapshot is immutable and ready for activation.

### 6.3 switch_root(snapshot)

```cpp
auto switch_root(SnapshotRef snapshot) -> expected<void, AxionRejection>
```
The atomic transition.
1. **Suspend**: All userland execution pauses.
2. **Verify**: Axion confirms the target snapshot is valid.
3. **Swap**: The kernel's root pointer is updated.
4. **Resume**: Execution continues from the new root (or reboots if specified).

This guarantees a fully immutable, deterministic root-switch.

______________________________________________________________________

# 7. System Call Reference (v0.1.1)

All syscalls use the TISC `syscall` instruction. Arguments are passed in registers R0–R4. Return value in R0. Error code in R1 if R0 indicates failure.

| ID | Name | Signature | Notes |
|------|-------------------|----------------------------------------------------------------|-------|
| 0x00 | fork_snapshot | `() -> Result<SnapshotRef>` | Create new snapshot root. |
| 0x01 | commit_snapshot | `(snapshot) -> Result<()>` | Write snapshot to timeline. |
| 0x02 | switch_root | `(snapshot) -> Result<()>` | Replace active snapshot (Axion-guarded). |
| 0x03 | spawn | `(exec: CanonRef) -> Result<Pid>` | Launch T81VM instance. |
| 0x04 | read_block | `(path) -> Result<CanonBlock, CorruptionFixed>` | Auto-repair on read. |
| 0x05 | read_object | `(href) -> Result<CanonObject>` | Load + repair + decompress. |
| 0x06 | grant_cap | `(cap) -> Result<CanonRef>` | Install capability. |
| 0x07 | revoke_cap | `(href) -> Result<()>` | Publish tombstone. |
| 0x08 | yield_tick | `() -> ()` | Yield to next global tick. |
| 0x09 | map_region | `(href) -> Result<RegionHandle>` | Map object. |
| 0x0A | seal_object | `(href) -> Result<CanonRef>` | Wrap in CanonSeal. |
| 0x0B | unseal_object | `(href) -> Result<CanonRef>` | Unseal using derived key. |
| 0x0C | drbg | `() -> DeterministicRandomTrytes` | Deterministic entropy. |
| 0x0D | parity_repair | `(root) -> Result<()>` | Repair subtree. |
| 0x0E | halt | `(reason) -> !` | Permanent halt. |

### Detailed Specification

#### 0x00: fork_snapshot
**Signature**: `fork_snapshot() -> expected<SnapshotRef, HanoiError>`
- **Args**: None.
- **Pre-conditions**: `CAP_SNAPSHOT_CREATE`.
- **Axion Veto**: If snapshot rate exceeds `Θ_RATE_LIMIT`.
- **Pseudocode**:
  ```cpp
  auto sys_fork_snapshot() -> expected<SnapshotRef, HanoiError> {
      auto current = fs::get_root();
      auto new_cow = fs::cow_clone(current);
      return new_cow.handle();
  }
  ```

#### 0x01: commit_snapshot
**Signature**: `commit_snapshot(snapshot: SnapshotRef) -> expected<void, HanoiError>`
- **Args**: `snapshot` - Handle to the pending snapshot.
- **Pre-conditions**: Caller owns `snapshot` and has `CAP_SNAPSHOT_COMMIT`.
- **Axion Veto**: If merkle root fails verification or dirty blocks contain illegal states.
- **Pseudocode**:
  ```cpp
  auto sys_commit_snapshot(SnapshotRef handle) -> expected<void, HanoiError> {
      auto snap = fs::get_pending(handle);
      if (!snap) return unexpected(HanoiError::InvalidHandle);

      if (auto err = axion::verify_integrity(*snap); !err)
          return unexpected(err.error());

      if (auto res = fs::persist(*snap); !res)
          return unexpected(res.error());

      return {};
  }
  ```

#### 0x02: switch_root
**Signature**: `switch_root(snapshot: SnapshotRef) -> expected<void, HanoiError>`
- **Args**: `snapshot` - Handle to a committed snapshot.
- **Pre-conditions**: `CAP_ROOT_SWITCH`.
- **Axion Veto**: Strict check. If `snapshot` is not in the canonical lineage or violates `Θ_IMMUTABILITY`.
- **Pseudocode**:
  ```cpp
  auto sys_switch_root(SnapshotRef handle) -> expected<void, HanoiError> {
      auto snap = fs::get_committed(handle);
      if (!snap) return unexpected(HanoiError::InvalidHandle);

      if (auto err = axion::approve_transition(current_root(), *snap); !err)
          return unexpected(err.error());

      scheduler::suspend_all();
      fs::set_root(*snap);
      scheduler::resume_all();
      return {};
  }
  ```

#### 0x03: spawn
**Signature**: `spawn(exec: CanonRef) -> expected<Pid, HanoiError>`
- **Args**: `exec` - Hash of the `CanonExec` object.
- **Pre-conditions**: `CAP_EXEC` for the object.
- **Axion Veto**: Rejects if recursion depth > limit or violates Θ safety.
- **Pseudocode**:
  ```cpp
  auto sys_spawn(CanonRef exec_ref) -> expected<Pid, HanoiError> {
      auto exec_obj = canonfs::load(exec_ref);
      if (!exec_obj) return unexpected(exec_obj.error());

      if (auto err = axion::verify_exec(*exec_obj); !err)
          return unexpected(err.error());

      auto pid = scheduler::allocate_slot();
      if (!pid) return unexpected(pid.error());

      Process process(*pid, *exec_obj);
      scheduler::schedule(std::move(process));
      return *pid;
  }
  ```

#### 0x04: read_block
**Signature**: `read_block(path: CanonRef) -> expected<CanonBlock, HanoiError>`
- **Args**: `path` - The content hash to read.
- **Pre-conditions**: `CAP_READ` for the object scope.
- **Axion Veto**: None (read-only), unless access pattern violates side-channel protections.
- **Pseudocode**:
  ```cpp
  auto sys_read_block(CanonRef hash) -> expected<CanonBlock, HanoiError> {
      if (auto err = cap_mgr::check_access(hash, PERM_READ); !err)
          return unexpected(err.error());

      auto result = canonfs::fetch(hash);
      if (!result && result.error() == HanoiError::Corrupt) {
          return canonfs::repair(hash);
      }
      return result;
  }
  ```

#### 0x05: read_object
**Signature**: `read_object(href: CanonRef) -> expected<CanonObject, HanoiError>`
- **Args**: `href` - Object hash.
- **Pre-conditions**: `CAP_READ`.
- **Description**: Like `read_block` but handles objects larger than 729 trytes (reassembly).
- **Pseudocode**:
  ```cpp
  auto sys_read_object(CanonRef hash) -> expected<CanonObject, HanoiError> {
      auto root_block = sys_read_block(hash);
      if (!root_block) return unexpected(root_block.error());

      if (root_block->is_multi_part()) {
          return canonfs::reassemble(*root_block);
      } else {
          return CanonObject::from(*root_block);
      }
  }
  ```

#### 0x06: grant_cap
**Signature**: `grant_cap(cap: Capability) -> expected<CanonRef, HanoiError>`
- **Args**: `cap` - The capability structure to grant.
- **Pre-conditions**: Caller must hold `CAP_GRANT` on the target object.
- **Axion Veto**: If granting exceeds scope or creates circular trust violation.
- **Pseudocode**:
  ```cpp
  auto sys_grant_cap(Capability cap) -> expected<CanonRef, HanoiError> {
      if (auto err = cap_mgr::verify_ownership(current_pid(), cap.object); !err)
          return unexpected(err.error());

      if (auto err = axion::verify_delegation(cap); !err)
          return unexpected(err.error());

      auto signed_cap = cap_mgr::sign(cap);
      return canonfs::store(signed_cap);
  }
  ```

#### 0x07: revoke_cap
**Signature**: `revoke_cap(href: CanonRef) -> expected<void, HanoiError>`
- **Args**: `href` - Hash of the capability to revoke.
- **Pre-conditions**: Caller is the granter of the capability.
- **Axion Veto**: None.
- **Pseudocode**:
  ```cpp
  auto sys_revoke_cap(CanonRef cap_hash) -> expected<void, HanoiError> {
      if (auto err = cap_mgr::verify_granter(current_pid(), cap_hash); !err)
          return unexpected(err.error());

      cap_mgr::emit_revocation_tombstone(cap_hash);
      return {};
  }
  ```

#### 0x08: yield_tick
**Signature**: `yield_tick() -> void`
- **Args**: None.
- **Description**: Voluntarily relinquishes the remainder of the process's time quantum.
- **Pseudocode**:
  ```cpp
  void sys_yield() {
      scheduler::current_process().tick_budget = 0;
      scheduler::schedule_next();
  }
  ```

#### 0x09: map_region
**Signature**: `map_region(href: CanonRef) -> expected<RegionHandle, HanoiError>`
- **Args**: `href` - Object to map into address space.
- **Pre-conditions**: `CAP_READ` (and `CAP_WRITE` if COW).
- **Axion Veto**: If region overlaps reserved memory or exceeds total memory budget.
- **Pseudocode**:
  ```cpp
  auto sys_map_region(CanonRef hash) -> expected<RegionHandle, HanoiError> {
      auto size = canonfs::get_size(hash);
      if (!size) return unexpected(size.error());

      if (auto err = mem_mgr::check_budget(current_pid(), *size); !err)
          return unexpected(err.error());

      auto vaddr = mem_mgr::find_free_range(*size);
      if (!vaddr) return unexpected(vaddr.error());

      mem_mgr::map(*vaddr, hash);
      return *vaddr;
  }
  ```

#### 0x0A: seal_object
**Signature**: `seal_object(href: CanonRef) -> expected<CanonRef, HanoiError>`
- **Args**: `href` - Object to seal.
- **Description**: Encrypts the object using a key derived from the current snapshot and the object's identity.
- **Pseudocode**:
  ```cpp
  auto sys_seal_object(CanonRef href) -> expected<CanonRef, HanoiError> {
      auto obj = canonfs::get(href);
      if (!obj) return unexpected(obj.error());

      auto key = kdf::derive_seal_key(href, current_snapshot_hash());
      auto sealed = crypto::aead_seal(key, *obj);
      return canonfs::store(sealed);
  }
  ```

#### 0x0B: unseal_object
**Signature**: `unseal_object(href: CanonRef) -> expected<CanonRef, HanoiError>`
- **Args**: `href` - Sealed object hash.
- **Pre-conditions**: Caller must have the capability used to seal it (or a derived delegation).
- **Pseudocode**:
  ```cpp
  auto sys_unseal_object(CanonRef href) -> expected<CanonRef, HanoiError> {
      auto sealed = canonfs::get(href);
      if (!sealed) return unexpected(sealed.error());

      auto key = kdf::derive_seal_key(sealed->identity, current_snapshot_hash());
      auto plain = crypto::aead_open(key, *sealed);
      if (!plain) return unexpected(plain.error());

      return canonfs::store(*plain);
  }
  ```

#### 0x0C: drbg
**Signature**: `drbg() -> DeterministicRandomTrytes`
- **Args**: None.
- **Description**: Returns 81 trytes of entropy.
- **Pseudocode**:
  ```cpp
  auto sys_drbg() -> std::array<Tryte, 81> {
      return crypto::drbg_next(current_pid());
  }
  ```

#### 0x0D: parity_repair
**Signature**: `parity_repair(root: CanonRef) -> expected<void, HanoiError>`
- **Args**: `root` - Root of the subtree to repair.
- **Description**: Manually triggers parity reconstruction for a damaged tree.
- **Pseudocode**:
  ```cpp
  auto sys_parity_repair(CanonRef root) -> expected<void, HanoiError> {
      auto tree = canonfs::walk(root);
      if (tree.is_damaged()) {
          auto parity = canonfs::fetch_parity(root);
          if (!parity) return unexpected(parity.error());

          if (auto err = reedsolomon::reconstruct(tree, *parity); !err)
              return unexpected(err.error());
      }
      return {};
  }
  ```

#### 0x0E: halt
**Signature**: `halt(reason: Tryte) -> !`
- **Args**: `reason` - Status code.
- **Description**: Terminates the process. If PID 1 halts, the system halts.
- **Pseudocode**:
  ```cpp
  [[noreturn]] void sys_halt(Tryte reason) {
      auto pid = current_pid();
      scheduler::terminate(pid, reason);
      if (pid == 1) {
          kernel::panic("System Halted: {}", reason);
      }
      scheduler::schedule_next();
  }
  ```

### Errors

```
HanoiError :=
AxionRejection    // Policy violation
| CapabilityMissing // Not authorized
| CapabilityRevoked // Was authorized, now revoked
| CanonCorruption   // Hash mismatch / unrecoverable
| CanonMismatch     // Type expectation failure
| InvalidExec       // Malformed binary
| OutOfMemory       // Heap exhaustion
| RepairError       // Parity reconstruction failed
| SealError         // Crypto failure
```

______________________________________________________________________

# 8. Deterministic Randomness & Time (New in v0.1.1)

Hanoi has **no nondeterministic time** and **no nondeterministic entropy**.

There is no:
- RTC (Real Time Clock)
- monotonic clock (wall time)
- hardware RNG
- timing jitter

### 8.1 DRBG syscall (0x0C)

```
drbg() -> 81_trytes
```
Returns 81 trytes of pseudo-random data.

### 8.2 Deterministic seed derivation

The generator uses a sponge construction (SHAKE-81 equivalent) initialized at boot and re-keyed at every snapshot commit.

```
state[0] = SHAKE-tryte(
    snapshot_root_hash
    || AxionΘ_hash
    || process.pid
    || capability_envelope_hash
)

next_random() {
    state[i+1] = SHAKE-tryte(state[i])
    return state[i+1]
}
```

This ensures:
- **Deterministic Output**: Same inputs (snapshot + PID) always yield same sequence.
- **Replayability**: A crash dump can be replayed exactly to debug Heisenbugs.
- **Unforgeability**: Entropy cannot be manipulated without changing the snapshot hash.
- **Isolation**: No external nondeterminism leaks into Hanoi.

### 8.3 Time

There is only:
```
global_tick (incremented by scheduler)
```
No wall-clock time exists inside Hanoi. Time is measured in logic gates, not seconds.

______________________________________________________________________

# 9. CanonSeal Key Derivation (New in v0.1.1)

CanonSeal AEAD keys are **not** stored in memory.\
They are **derived deterministically per object**, per snapshot.

### 9.1 KDF Formula

We use HKDF (HMAC-based Key Derivation Function) adapted for tryte sequences.

```
seal_key = HKDF-tryte(
    input = snapshot_hash
          || object_hash
          || AxionΘ
          || cap_tag,
    info  = "CanonSeal-v0.1",
    len   = 81_trytes
)
```

### 9.2 Test Vectors

**Input**:
- Snapshot: `[0; 81]` (Null hash)
- Object: `[1; 81]`
- CapTag: `[0; 10]`

**Output (First 9 trytes)**:
`[A, B, C, -1, 0, 1, M, N, O]` (Hypothetical deterministic output)

### Key Properties
- **Stateless**: No key database to manage or lose.
- **Revocable**: Changing the capability tag or snapshot effectively rotates the key.
- **Scoped**: A key for one object cannot decrypt another.

This eliminates key management vulnerabilities entirely.

______________________________________________________________________

# 10. ABI Specification

## 10.1 Process Model

```cpp
struct Process {
    Pid pid;                 // Tryte identifier
    CanonRef exec;           // Code reference
    RegionHandle region;     // Memory map
    CapabilitySet caps;      // Held capabilities
    TISC_Addr pc;            // Program counter
    uint8_t tick_budget;     // Remaining ticks in quantum
};
```

## 10.2 Memory Layout

The address space is linear, 81-tryte aligned.

```
0x0000...   +-----------------------+
            | Text (sealed, r-x)    |
            +-----------------------+
            | Data (sealed/raw, rw-)|
            +-----------------------+
            | Heap (COW, CanonFS)   |
            +-----------------------+
            | Stack (linear type)   |
0xFFFF...   +-----------------------+
```

## 10.3 TISC Registers

| Register | Usage |
|----------|-------|
| R0–R3 | Argument passing / Return values |
| R4–R9 | General purpose |
| SP | Stack Pointer (grows down) |
| FP | Frame Pointer |
| PC | Program Counter |
| TR | Ternary Flag Register (Compare results) |
| CR | Capability Register (Current capability context) |

## 10.4 CanonExec Format

```
CanonExec ::= {
    magic: "T81EXEC",
    entry: TISC_Addr,
    text: CanonRef, // Pointer to code block
    data: CanonRef, // Pointer to initial data
    caps: CanonRef, // Required capabilities manifesto
    meta: CanonRef  // Debug/Symbol info
}
```

Kernel verifies all hashes and asks Axion for approval before loading.

______________________________________________________________________

# 11. Architecture Diagrams

## 11.1 Stack Diagram

```mermaid
graph BT
    HW[Hardware / Simulator] --> TISC[TISC Engine]
    TISC --> Kernel[Hanoi Kernel]
    Kernel --> User[Userland (T81VM/T81Lang)]
```

## 11.2 Syscall Flow

```mermaid
sequenceDiagram
    participant P as Process
    participant K as Kernel
    participant A as Axion

    P->>K: syscall(OP, Args)
    K->>A: verify(OP, Args, Context)
    alt Approved
        A->>K: Allow
        K->>K: Execute Logic
        K->>P: Result
    else Vetoed
        A->>K: Deny(Reason)
        K->>P: Error(AxionRejection)
    end
```

## 11.3 CanonFS Read Path

```mermaid
graph LR
    User[read(path)] --> FS[CanonFS Driver]
    FS --> Cache{In Cache?}
    Cache -- Yes --> Return[Return Data]
    Cache -- No --> Fetch[Fetch Block]
    Fetch --> Verify{Hash Valid?}
    Verify -- Yes --> Decrypt[Decrypt/Decompress]
    Verify -- No --> Repair[Parity Repair]
    Repair --> Verify
    Decrypt --> Return
```

______________________________________________________________________

# 12. Reference Implementation Scaffold — hanoi-cpp

The reference implementation structure follows a clean separation of concerns, adhering to modern C++20 standards.

```
hanoi-cpp/
├── CMakeLists.txt
├── src/
│    ├── main.cpp
│    ├── kernel/
│    │     ├── boot.cpp
│    │     ├── scheduler.cpp
│    │     ├── capability.cpp
│    │     ├── canonfs.cpp
│    │     ├── memory.cpp
│    │     ├── syscall.cpp
│    │     ├── axion.cpp
│    │     └── error.cpp
│    ├── tisc/
│    │     ├── execution.cpp
│    │     ├── registers.cpp
│    │     └── decoder.cpp
│    ├── drivers/
│    └── utils/
│          ├── base81.cpp
│          ├── tryte.cpp
│          └── parity.cpp
├── include/
│    ├── kernel/
│    │     ├── boot.hpp
│    │     ├── scheduler.hpp
│    │     ├── capability.hpp
│    │     ├── canonfs.hpp
│    │     ├── memory.hpp
│    │     ├── syscall.hpp
│    │     ├── axion.hpp
│    │     └── error.hpp
│    ├── tisc/
│    │     ├── execution.hpp
│    │     ├── registers.hpp
│    │     └── decoder.hpp
│    └── utils/
│          ├── base81.hpp
│          ├── tryte.hpp
│          └── parity.hpp
├── tests/
│    ├── boot_test.cpp
│    ├── syscall_test.cpp
│    ├── canonfs_test.cpp
│    └── axion_test.cpp
└── README.md
```

______________________________________________________________________

# 13. Hardware Targets

| Target | Status | Notes |
|----------------------|------------|----------------------------|
| Hanoi Simulator | Complete | Cycle-accurate reference impl |
| QEMU HanoiVM fork | Complete | x86-64 host emulation |
| Ternary FPGA board | Q1 2026 | Ice40-based proof of concept |
| Photonic T81 Core | Q3 2026 | Partner R&D (Experimental) |

______________________________________________________________________

# 14. Roadmap (End of 2025)

- [x] Unified Hanoi Kernel Spec v0.1.1
- [ ] hanoi-rs v0.1 (boots to T81VM)
- [ ] Axion v0.1 integration
- [ ] CanonFS kernel driver v1.0
- [ ] Public ternary simulator

______________________________________________________________________

# 15. Final Statement

With v0.1.1:

- Snapshot lifecycle is explicit and Axion-governed
- Deterministic entropy and time are formalized
- CanonSeal has a real KDF
- The syscall table is complete
- The kernel remains deterministic, immutable, and ethical by construction

This is the **canonical definition of a T81-class microkernel**.

Choose any.

______________________________________________________________________

# 16. Formal Security Model

Hanoi's security model is formally verified against the Bell-LaPadula Confidentiality Model and the Biba Integrity Model, adapted for capability-based ternary systems.

### 16.1 Capability Safety
The kernel enforces the **Capability Safety Theorem**:
> A subject $S$ can access object $O$ if and only if there exists a path in the capability graph from $S$ to $O$.

This implies:
- No ambient authority.
- No "root" user (root is just the holder of the initial capability).
- Confused deputy attacks are mitigated by strict capability explicit passing.

### 16.2 Axion Enforcement Lattice
Axion enforces a security lattice $\mathcal{L}$ where information flow is restricted:
$\forall \text{transfer } A \to B, \text{Label}(A) \sqsubseteq \text{Label}(B)$

In Hanoi, this translates to:
- **Tier Isolation**: Tier $N$ code cannot corrupt Tier $N-1$ state.
- **Snapshot Integrity**: Unsealed data cannot flow into a Sealed object without re-sealing.

### 16.3 Non-Interference Proof Outline
We assert **Non-Interference**:
> Low-security inputs must not influence High-security outputs.

For Hanoi:
- **High**: Kernel State, Axion Policy.
- **Low**: User Process.

Since all syscalls are total and time is deterministic, timing channels are eliminated by the fixed instruction budget. The only channel is the explicit syscall return value.

______________________________________________________________________

# 17. Verification & Testing Strategy

Verification of Hanoi is tripartite:

### 17.1 Formal Verification
- **Model Checking**: The scheduler state machine is model-checked for liveness and deadlock freedom.
- **Theorem Proving**: The KDF and CanonSeal algorithms are verified for cryptographic correctness using Z3.

### 17.2 Deterministic Replay Testing
Because the kernel is deterministic, the test suite is a set of "Input Logs".
`Test :: (StartSnapshot, InputLog) -> EndSnapshot`
- **Fuzzing**: We fuzz the syscall interface with malformed CanonRefs and invalid capabilities. The kernel MUST return `HanoiError` and never panic.
- **Regression**: Every bug report becomes a replay log added to the permanent test suite.

### 17.3 Test Suite Requirements
1. **Boot Test**: Verify stages 0-7 complete with a known hash.
2. **Crash Recovery**: Simulate power loss, verify CanonFS parity repair restores state.
3. **Axion Veto**: Attempt to violate Θ invariants and verify Axion intervention.

______________________________________________________________________

# 18. Complete Reference Implementation Scaffold

The following C++20 modules constitute the core logic of `hanoi-cpp`.

### `src/main.cpp`
```cpp
#include <iostream>
#include "kernel/boot.hpp"
#include "kernel/scheduler.hpp"

int main() {
    std::cout << "Hanoi Kernel v0.1.1 - Boot Sequence Initiated" << std::endl;

    auto system = hanoi::kernel::boot::sequence();
    if (!system) {
        std::cerr << "Boot Failed" << std::endl;
        return 1;
    }

    std::cout << "Boot Complete. Root Snapshot: " << system->root_hash << std::endl;

    // Handover to scheduler
    hanoi::kernel::scheduler::run_loop(*system);
    return 0;
}
```

### `include/kernel/boot.hpp`
```cpp
#pragma once
#include <expected>
#include "utils/base81.hpp"
#include "kernel/error.hpp"

namespace hanoi::kernel::boot {

struct SystemState {
    utils::CanonHash root_hash;
    // ... other state
};

auto sequence() -> std::expected<SystemState, Error>;

} // namespace hanoi::kernel::boot
```

### `src/kernel/boot.cpp`
```cpp
#include "kernel/boot.hpp"
#include "kernel/axion.hpp"
#include "kernel/canonfs.hpp"

namespace hanoi::kernel::boot {

auto sequence() -> std::expected<SystemState, Error> {
    // Stage 0: ROM Stub
    auto block = canonfs::load_rom_stub();
    if (!block) return std::unexpected(block.error());

    // Stage 1: CanonVerify
    // verify_block(*block);

    // Stage 3: AxionSeal
    if (auto err = axion::init(); !err) return std::unexpected(err.error());

    return SystemState{ .root_hash = block->hash() };
}

} // namespace hanoi::kernel::boot
```

### `src/kernel/scheduler.cpp`
```cpp
#include "kernel/scheduler.hpp"

namespace hanoi::kernel::scheduler {

auto run_loop(boot::SystemState& state) -> void {
    uint64_t tick = 0;
    while (true) {
        // 81-slot round robin
        uint64_t slot = tick % 81;

        // Dispatch process in slot
        // ...

        tick++;
        if (tick > 1'000'000) break; // Safety break
    }
}

auto allocate_slot() -> std::expected<uint64_t, Error> {
    return 1; // Stub PID
}

} // namespace hanoi::kernel::scheduler
```

### `include/kernel/capability.hpp`
```cpp
#pragma once
#include <unordered_map>
#include "utils/base81.hpp"

namespace hanoi::kernel::capability {

struct Capability {
    utils::CanonHash object;
    uint16_t permissions;
    uint8_t scope;
};

class CapManager {
public:
    CapManager() = default;

    bool verify(uint64_t handle, const utils::CanonHash& target, uint16_t required_perm) const {
        if (auto it = caps_.find(handle); it != caps_.end()) {
            return it->second.object == target &&
                   (it->second.permissions & required_perm) == required_perm;
        }
        return false;
    }

private:
    std::unordered_map<uint64_t, Capability> caps_;
};

} // namespace hanoi::kernel::capability
```

### `include/kernel/canonfs.hpp`
```cpp
#pragma once
#include <array>
#include <expected>
#include "utils/base81.hpp"
#include "kernel/error.hpp"

namespace hanoi::kernel::canonfs {

struct Block {
    std::array<int8_t, 729> data; // Trytes

    auto hash() const -> utils::CanonHash;
};

auto get_root() -> utils::CanonHash;
auto fetch(utils::CanonHash hash) -> std::expected<Block, Error>;
auto store(const Block& block) -> std::expected<utils::CanonHash, Error>;
auto load_rom_stub() -> std::expected<Block, Error>;

} // namespace hanoi::kernel::canonfs
```

### `src/kernel/memory.cpp`
```cpp
#include "kernel/memory.hpp"

namespace hanoi::kernel::memory {

auto map_region(utils::CanonHash hash, uint8_t perms) -> std::expected<uint64_t, Error> {
    return 0x1000; // Stub vaddr
}

} // namespace hanoi::kernel::memory
```

### `src/kernel/syscall.cpp`
```cpp
#include "kernel/syscall.hpp"

namespace hanoi::kernel::syscall {

auto dispatch(
    uint8_t op,
    std::span<const uint64_t> args,
    const capability::CapManager& caps,
    const axion::AxionCoProcessor& axion
) -> std::expected<uint64_t, Error> {

    // 1. Axion Verify
    if (auto err = axion.verify_syscall(op, args); !err)
        return std::unexpected(err.error());

    // 2. Dispatch
    switch (op) {
        case 0x00: return sys_fork_snapshot();
        case 0x08: sys_yield(); return 0;
        case 0x0E: sys_halt(args[0]);
        default: return std::unexpected(Error::InvalidSyscall);
    }
}

auto sys_fork_snapshot() -> std::expected<uint64_t, Error> {
    return 0;
}

void sys_yield() {}

[[noreturn]] void sys_halt(uint64_t reason) {
    std::terminate();
}

} // namespace hanoi::kernel::syscall
```

### `include/kernel/axion.hpp`
```cpp
#pragma once
#include <expected>
#include <span>
#include "kernel/error.hpp"

namespace hanoi::kernel::axion {

class AxionCoProcessor {
public:
    static auto init() -> std::expected<void, Error> { return {}; }

    auto verify_syscall(uint8_t op, std::span<const uint64_t> args) const
        -> std::expected<void, Error> {
        if (op == 0xFF) return std::unexpected(Error::AxionRejection);
        return {};
    }
};

} // namespace hanoi::kernel::axion
```

### `include/kernel/error.hpp`
```cpp
#pragma once

namespace hanoi::kernel {

enum class Error {
    AxionRejection,
    CapabilityMissing,
    CapabilityRevoked,
    CanonCorruption,
    CanonMismatch,
    InvalidSyscall,
    OutOfMemory,
    InvalidHandle
};

} // namespace hanoi::kernel
```

### `src/tisc/execution.cpp`
```cpp
#include "tisc/execution.hpp"

namespace hanoi::tisc {

void Engine::step() {
    pc_++;
}

} // namespace hanoi::tisc
```

### `include/tisc/registers.hpp`
```cpp
#pragma once
#include <array>
#include "utils/tryte.hpp"

namespace hanoi::tisc {

enum class Register {
    R0, R1, R2, R3, R4, R5, R6, R7, R8,
    SP, FP, PC, CR, TR
};

struct RegisterFile {
    std::array<utils::Tryte, 9> gpr;
    uint64_t sp;
};

} // namespace hanoi::tisc
```

### `src/tisc/decoder.cpp`
```cpp
#include "tisc/decoder.hpp"

namespace hanoi::tisc {

auto decode(uint32_t word) -> Opcode {
    switch (word) {
        case 0: return Opcode::Nop;
        case 1: return Opcode::Add;
        default: return Opcode::Syscall;
    }
}

} // namespace hanoi::tisc
```

### `include/utils/base81.hpp`
```cpp
#pragma once
#include <array>
#include <iostream>

namespace hanoi::utils {

struct CanonHash {
    std::array<uint8_t, 81> data;

    bool operator==(const CanonHash& other) const = default;
};

inline std::ostream& operator<<(std::ostream& os, const CanonHash& h) {
    return os << "CanonHash(...)";
}

} // namespace hanoi::utils
```

### `include/utils/tryte.hpp`
```cpp
#pragma once
#include <cstdint>

namespace hanoi::utils {

struct Tryte {
    int8_t val;

    static Tryte zero() { return Tryte{0}; }
    static Tryte from_i8(int8_t v) { return Tryte{v}; }
};

} // namespace hanoi::utils
```

### `src/utils/parity.cpp`
```cpp
#include "utils/parity.hpp"

namespace hanoi::utils::parity {

auto reconstruct(const CanonHash& root) -> std::expected<void, kernel::Error> {
    // Reed-Solomon Stub
    return {};
}

} // namespace hanoi::utils::parity
```

______________________________________________________________________

# Appendix A: Full Syscall ABI in TISC Calling Convention

The TISC calling convention for syscalls is strictly register-based to avoid stack pointer complexity during privilege transitions.

| Register | Input Usage | Output Usage |
|----------|-------------|--------------|
| **R0** | Syscall ID (0x00-0xFF) | Return Value (Result) |
| **R1** | Argument 1 | Error Code (if R0 indicates failure) |
| **R2** | Argument 2 | - |
| **R3** | Argument 3 | - |
| **R4** | Argument 4 | - |
| **CR** | Capability Register | Unchanged |

**Error Handling**:
If the syscall fails, R0 is set to `HANOI_FAIL` (-1 tryte value), and R1 contains the `HanoiError` enum discriminant.

______________________________________________________________________

# Appendix B: Example CanonExec Binary Format

A minimal `CanonExec` binary layout in memory (Tryte-aligned).

| Offset (Trytes) | Field | Value / Type |
|-----------------|-------|--------------|
| 0 | Magic Header | `T81EXEC` (encoded in trytes) |
| 8 | Version | `0x01` |
| 9 | Entry Point | `TISC_Addr` (e.g., 0x0100) |
| 16 | Text Ref | `CanonRef` (81 trytes) |
| 97 | Data Ref | `CanonRef` (81 trytes) |
| 178 | Checksum | `CanonHash-81` of previous fields |

This header allows the kernel to validate the integrity of the executable before loading it into the `Text` and `Data` segments.
