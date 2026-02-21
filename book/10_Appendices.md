# Chapter 10: Appendices

## 10.1 What Is Not Yet Implemented

While the core T81VM, TISC, and Axion Kernel are stable, several advanced features described in the specification are currently in experimental or placeholder status.

### 10.1.1 Network Stack (Tier 4)
*   **Status**: Placeholder (Stubbed Opcodes).
*   **Missing**: Real-world P2P networking, DHT implementation, cryptographic handshake.
*   **Current Behavior**: `NSend`/`NRecv` log Axion events but do not transmit data.

### 10.1.2 Distributed Consensus
*   **Status**: Experimental.
*   **Missing**: Byzantine Fault Tolerance (BFT) consensus algorithm.
*   **Current Behavior**: Gossip protocol merges state using local logical clocks without global consensus verification.

### 10.1.3 Hardware Acceleration
*   **Status**: Research.
*   **Missing**: FPGA/ASIC offloading for ternary arithmetic.
*   **Current Behavior**: Pure software emulation (`dmath`).

## 10.2 Threat Model and Determinism Attack Surface

The T81 security model assumes a **Hostile Host Environment**.

### 10.2.1 Host Interference
*   **Threat**: The OS scheduler preempts the VM thread non-deterministically.
*   **Mitigation**: T81VM uses logical ticks (Lamport timestamps) for all time-based logic. Wall-clock time is inaccessible to TISC code.

### 10.2.2 Time-Based Attacks
*   **Threat**: Observing execution time to infer secret data (Timing Side-Channel).
*   **Mitigation**: The Axion Trace logs *logical* operations, not physical time. However, strict constant-time execution for all opcodes is **not yet guaranteed** on commodity hardware.

### 10.2.3 RNG Contamination
*   **Threat**: Injecting host entropy (`/dev/random`) into the VM.
*   **Mitigation**: T81VM has no opcode to read host entropy. All randomness must be seeded via the input vector $I$.

### 10.2.4 Memory Layout Variance (ASLR)
*   **Threat**: Pointers leaking address space layout.
*   **Mitigation**: TISC code operates on logical handles and segment offsets. Physical addresses are never exposed to the guest program.

## 10.3 Glossary

*   **Axion**: The safety kernel and policy engine.
*   **CanonFS**: The content-addressable filesystem.
*   **dmath**: Deterministic software-defined math library.
*   **Gossip**: The protocol for distributed state synchronization.
*   **JIT**: Just-In-Time compilation (Trace-based).
*   **Lamport Tick**: A logical clock counter.
*   **TISC**: Ternary Instruction Set Computer.
*   **Trit**: Base-3 digit.
*   **Tryte**: Sequence of trits (usually 4).
