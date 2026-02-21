# Chapter 8: Documentation and Specifications

The T81 Foundation maintains a set of "Normative Specifications" in the `spec/` directory. These documents are the ultimate source of truth for the project. Code is considered "correct" only if it adheres to these specs.

## 8.1 Core Specifications

*   **`t81-data-types.md`**: Defines the bit-level layout of all data types. It mandates canonical forms (e.g., how a BigInt is packed into memory) to ensure that data can be serialized and hashed consistently.
*   **`tisc-spec.md`**: The Instruction Set Architecture. It defines the numeric opcode values, operand modes, and the exact semantic effect of every instruction on the VM state.
*   **`t81vm-spec.md`**: Defines the Virtual Machine's environment, including memory segmentation, error handling (faults vs. traps), and the boot process.
*   **`axion-kernel.md`**: Defines the Axion safety layer, including the format of policies and the specific hooks that the VM must call.

## 8.2 CanonFS (Canonical File System)

Defined in `spec/canonfs-spec.md`, CanonFS is the native storage layer for T81.

### 8.2.1 Philosophy
CanonFS replaces traditional hierarchical file systems (POSIX) with a **content-addressed**, **immutable** object store.
*   **Immutable**: Once a file is written, it cannot be changed. Updating a file creates a new version with a new hash.
*   **Content-Addressed**: Files are identified by the hash of their content (`CanonHash-81`), not by a user-defined name.
*   **Capability-Secured**: Access requires a cryptographic capability, not just a permission bit.

### 8.2.2 CanonBlock
The atomic unit of storage is the **CanonBlock**, which is exactly 729 trytes in size. This aligns with the base-81 architecture and allows for efficient addressing.

### 8.2.3 Directory Structure
Directories are simply sparse tensors (81x81) that map names (hashed) to object references.

## 8.3 Contribution Guidelines

As outlined in `CONTRIBUTING.md`:
1.  **Determinism is Law**: No PR will be accepted if it introduces nondeterminism.
2.  **Test Coverage**: New features must include unit tests.
3.  **Code Style**: Must pass `clang-format`.
4.  **License**: Contributions are licensed under MIT/GPL-3.0.

## 8.4 Security

The `SECURITY.md` policy emphasizes that while T81 is designed for safety, it is software. Vulnerabilities should be reported responsibly. The strict Axion sandbox aims to mitigate the impact of any VM bugs by preventing escape to the host system.
