# T81 Canonical Architecture

This document defines the authoritative architecture of the T81 system as implemented in the current codebase.

## 1. Execution Pipeline

The execution pipeline transforms source code into TISC bytecode, which is then executed by the T81 VM under strict Axion policy supervision.

```mermaid
flowchart LR
    Source[Source] --> Lexer[Lexer]
    Lexer --> Parser[Parser]
    Parser --> Semantic[Semantic Analyzer]
    Semantic --> IR[IR Generator]
    IR --> TISC[TISC Emitter]
    TISC --> VM[VM Interpreter]

    subgraph Runtime [src/vm]
        VM --> JIT{Hotspot?}
        JIT -- Yes --> Trace[ThreadedJitTrace]
        JIT -- No --> Exec[Execute Step]
        Trace --> Axion[Axion Policy]
        Exec --> Axion
    end

    subgraph Governance [src/axion]
        Axion --> Ethics{Ethics Check}
        Ethics -- Pass --> Commit[Commit State]
        Ethics -- Fail --> Trap[Trap / Halt]
    end
```

## 2. Runtime Boundary

The system strictly separates the deterministic core logic from the host-dependent surface and experimental features.

```mermaid
flowchart TD
    subgraph Core [Deterministic Core]
        Interpreter[Interpreter]
        JIT[ThreadedJitTrace]
        Axion[Axion Policy Engine]
        Mem[Memory Model]
        GC[Mark-and-Sweep GC]
    end

    subgraph Surface [Controlled Surface]
        CanonFS[CanonFS Driver]
        Weights[Weights Loader]
        Faults[Fault Injection]
    end

    subgraph Host [Host Boundary]
        Filesystem[std::filesystem]
        Alloc[std::vector/allocator]
    end

    Interpreter --> Mem
    Interpreter --> Axion
    Interpreter --> JIT
    Interpreter --> GC

    Interpreter --> CanonFS
    Interpreter --> Weights
    Interpreter --> Faults

    CanonFS --> Filesystem
    Weights --> Filesystem
    Mem --> Alloc
```

## 3. Cognitive Tier Escalation

The Cognitive Tier model defines the capabilities available to the runtime, escalating from basic symbolic manipulation to infinite series expansion, all supervised by Axion.

```mermaid
flowchart TD
    subgraph Tiers [Cognitive Tiers  src/cog ]
        T1[Tier 1: Symbolic]
        T2[Tier 2: Reflective]
        T3[Tier 3: Recursive]
        T4[Tier 4: Distributed  Stub ]
        T5[Tier 5: Infinite]
    end

    subgraph Supervision [Axion Supervision]
        Axion[Axion Policy Engine]
        Ethics[Ethics Principles]
        Limits[Recursion/Stack Limits]
        Promotion[Tier Promotion]
    end

    Interpreter[VM Interpreter] --> T1
    Interpreter --> T2
    Interpreter --> T3
    Interpreter --> T4
    Interpreter --> T5

    T1 --> Axion
    T2 --> Axion
    T3 --> Limits
    T3 --> Promotion
    T5 --> Axion

    Axion --> Ethics
    Promotion --> Axion
```

## Coverage Notes

- **Tier 4 (Distributed)**: The implementation files exist (`src/cog/tier4/distributed.cpp`), but the opcodes (`Merge`, `Gossip`, etc.) are currently stubs in `src/vm/vm.cpp` that log events but perform no network operations.
- **JIT**: The "JIT" is implemented as a tracing interpreter (`ThreadedJitTrace`), not a machine-code emitting JIT. It resides entirely within the deterministic core.
- **Network I/O**: Network opcodes (`NSend`, `NRecv`) are placeholders returning dummy values or strictly logging events, ensuring no nondeterministic network interaction occurs in the current implementation.
- **Weights**: Supported formats are GGUF and SafeTensors (via conversion), and the native T81W format.
```
