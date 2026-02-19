# T81 Foundation User Manual

The T81 Foundation stack is a sovereign-grade, ternary-native computing environment designed for deterministic execution, auditable safety, and efficient arithmetic. This manual provides a comprehensive guide to installing, using, and mastering the T81 toolchain.

## Table of Contents

- [Introduction](#introduction)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [CLI Reference](#cli-reference)
  - [Core Commands](#core-commands)
  - [Project Management](#project-management)
  - [Advanced Tools](#advanced-tools)
- [T81Lang Basics](#t81lang-basics)
- [Advanced Features](#advanced-features)
  - [Determinism & Reproducibility](#determinism--reproducibility)
  - [Axion Policy Engine](#axion-policy-engine)
  - [Weights & Tensors](#weights--tensors)
- [Troubleshooting](#troubleshooting)

---

## Introduction

T81 is built on three core pillars:
1.  **Ternary-Native Arithmetic**: Utilizing base-81 data types and balanced ternary logic for high-density, efficient computation.
2.  **Strict Determinism**: Guarantees bit-exact reproducibility across different hardware architectures (x86_64, ARM64) and platforms.
3.  **Axion Safety**: A runtime policy engine that enforces safety invariants and prevents unauthorized behavior.

The stack includes the **T81Lang** high-level language, the **TISC** (Ternary Instruction Set Computer) intermediate representation, and the **T81VM** (Virtual Machine).

---

## Installation

### Prerequisites
*   **C++ Compiler**: Clang 18+ or GCC 14+ (C++23 support required).
*   **Build System**: CMake 3.16+ and Ninja (recommended) or Make.
*   **Python**: Python 3.9+ (for scripts and reproducibility gates).

### Building from Source

Clone the repository and build using CMake:

```bash
git clone https://github.com/t81dev/t81-foundation.git
cd t81-foundation

# Configure
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --parallel
```

The primary executable will be located at `build/t81`. You can add this to your PATH or alias it:
```bash
alias t81=$(pwd)/build/t81
```

---

## Quick Start

### 1. Initialize a Project
Create a new T81 project with a standard directory structure:

```bash
t81 init my_project
cd my_project
```

This creates a `main.t81` file and a `README.md`.

### 2. Run the Program
Execute the program directly:

```bash
t81 run main.t81
```

### 3. Compile to Bytecode
For distribution or repeated execution, compile the source to TISC bytecode:

```bash
t81 compile main.t81 -o main.tisc
t81 run main.tisc
```

---

## CLI Reference

The `t81` command-line interface is the unified entry point for all tools.

Global Options:
*   `-v`, `--verbose`: Enable verbose diagnostic output.
*   `-q`, `--quiet`: Suppress non-error output.
*   `-h`, `--help`: Show help information.

### Core Commands

#### `compile`
Compiles T81Lang source code to TISC bytecode.

```bash
t81 compile <source.t81> [-o <output.tisc>]
```

#### `run`
Executes a T81Lang source file (by compiling it first) or a pre-compiled TISC bytecode file.

```bash
t81 run <file.t81|.tisc> [--policy <policy.apl>]
```
*   `--policy <policy.apl>`: Load and enforce an Axion safety policy during execution.

#### `check` / `lint`
Performs syntax and semantic analysis on a source file without generating code. Useful for quick validation.

```bash
t81 check <source.t81>
```

#### `repl`
Starts an interactive Read-Eval-Print Loop session.

```bash
t81 repl
```
*   Type T81Lang code and press Enter (twice for multi-line) to execute.
*   Commands: `:quit`, `:help`, `:load <file>`, `:history`, `:reset`, `:symbols`.

#### `debug`
Launches the interactive TISC debugger.

```bash
t81 debug <file.t81|.tisc>
```

#### `disasm`
Disassembles a TISC bytecode file into a human-readable format.

```bash
t81 disasm <file.tisc>
```

### Project Management

#### `init`
Scaffolds a new T81 project directory.

```bash
t81 init <project_name>
```

#### `pkg`
Manages T81 packages.

*   `t81 pkg init [name]`: Initialize a `package.t81` manifest.
*   `t81 pkg check`: Validate the current package manifest.

### Advanced Tools

#### `weights`
Manages machine learning model weights and tensors.

*   `import <file> [-o <out.t81w>] [--format <fmt>]`: Import weights from SafeTensors or GGUF formats into the native `.t81w` format.
*   `info <model.t81w>`: Display metadata, sparsity, and checksums for a model file.
*   `quantize <input> --to-gguf <output>`: Quantize weights (e.g., SafeTensors) to T3_K GGUF format for compatibility with other tools.

#### `policy`
Tools for the Axion Policy Engine.

*   `compile <file.apl> [-o <out.axionb>]`: Compile an Axion Policy Language (APL) file to binary policy format.
*   `run <file.apl|.axionb>`: Validate and load a policy to check for errors.

#### `trace`
Tools for analyzing execution traces (requires execution with tracing enabled).

*   `show <trace.txt>`: Visualize a trace file with color coding.
*   `diff <trace1.txt> <trace2.txt>`: Compare two traces to identify divergences.
*   `replay <file.tisc> <trace.txt>`: Re-execute a program and verify it matches the recorded trace bit-for-bit.

#### `repro-hash`
Runs the deterministic reproduction gate, hashing the execution outputs of test fixtures to verify cross-platform consistency.

```bash
t81 repro-hash [fixtures_dir]
```

#### `benchmark`
Runs the internal benchmark suite to measure performance.

```bash
t81 benchmark
```

---

## T81Lang Basics

T81Lang is a statically-typed language designed for ternary computing.

**Basic Syntax:**
```t81
// Variables
let x: i32 = 42;
let y = 81; // Type inferred

// Control Flow
if x > 0 {
    y = y + 1;
} else {
    y = y - 1;
}

loop {
    if y > 100 { break; }
    y = y + 1;
}

// Functions
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}
```

For a complete specification, refer to the [T81Lang Specification](../../spec/t81lang-spec.md).

---

## Advanced Features

### Determinism & Reproducibility
T81 guarantees that the same code produces the exact same output on any supported platform.
*   **Verification**: Use `t81 repro-hash` to generate a canonical hash of your environment's execution behavior.
*   **Floating Point**: T81 implements strict software-defined floating-point arithmetic to avoid hardware-specific deviations.

### Axion Policy Engine
Axion is a runtime kernel that monitors execution against defined safety policies. For details, see the [Axion Kernel Specification](../../spec/axion-kernel.md).
*   **Policies**: Define constraints like maximum instruction count, memory limits, or restricted opcodes in `.apl` files.
*   **Enforcement**: Pass a policy with `--policy` to `t81 run`. If the program violates the policy, the VM traps and execution halts.

### Weights & Tensors
T81 supports native tensor operations optimized for ternary hardware simulation.
*   **Format**: The `.t81w` format stores tensors with SHA3-512 checksums for integrity.
*   **Integration**: Use `t81 weights import` to bring in models from common formats like SafeTensors.

---

## Troubleshooting

*   **Build Failures**: Ensure you have a C++23 compliant compiler. If CMake fails, try clearing the `build/` directory and re-running configuration.
*   **AVX2 Warnings**: On non-x86 platforms (like Apple Silicon), you may see warnings about AVX2 being disabled. This is normal and does not affect correctness, only specific SIMD optimizations.
*   **Verification Failures**: If `repro-hash` fails, ensure your environment is not injecting external entropy (T81 is designed to be hermetic, but external file I/O can introduce variability).

For more help, consult the [Architecture Guide](../explanation/ARCHITECTURE.md) or open an issue on GitHub.
