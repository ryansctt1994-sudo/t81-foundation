# Chapter 6: Usage and CLI Guide

The primary interface to the T81 ecosystem is the `t81` command-line tool. This unified driver handles compilation, execution, debugging, and asset management.

## 6.1 The `t81` Command

**Usage**: `t81 <command> [options] [args]`

### Global Options
*   `-v, --verbose`: Enable verbose diagnostic output.
*   `-q, --quiet`: Suppress non-error output.
*   `-h, --help`: Show help message.

## 6.2 Core Commands

### `compile`
Compiles T81Lang source code into TISC bytecode.

```bash
t81 compile source.t81 -o output.tisc
```
*   **Input**: `.t81` source file or `.t81w` weights module.
*   **Output**: `.tisc` bytecode file.

### `run`
Executes a program. Can run TISC bytecode directly or compile-and-run source code.

```bash
t81 run program.tisc
t81 run source.t81 [--policy security.apl]
```
*   **--policy**: Optional path to an Axion Policy Language file to govern execution.

### `debug`
Starts the interactive debugger.

```bash
t81 debug program.tisc
```
*   Provides a step-by-step interface to inspect registers, memory, and stack.

### `disasm`
Disassembles TISC bytecode into a human-readable text format.

```bash
t81 disasm program.tisc
```
*   Useful for inspecting the output of the compiler or verifying bytecode.

### `check` / `lint`
Performs syntax and semantic analysis without generating code.

```bash
t81 check source.t81
```
*   Use this for fast feedback during development.

## 6.3 Model Management (`weights`)

The `weights` subcommand manages AI model files (`.t81w`, SafeTensors, GGUF).

### `import`
Imports weights from external formats into the native T81W format.

```bash
t81 weights import model.safetensors -o model.t81w
```
*   **--format**: Input format (`safetensors` or `gguf`). Default is safetensors.

### `quantize`
Quantizes a SafeTensors model into the `T3_K` ternary-optimized format (exported as GGUF).

```bash
t81 weights quantize model.safetensors --to-gguf model.gguf
```

### `info`
Displays metadata about a T81W file (parameter count, sparsity, format).

```bash
t81 weights info model.t81w
```

## 6.4 Axion Policy Tools (`policy`)

### `compile`
Compiles a policy text file (`.apl`) into binary policy format (`.axionb`).

```bash
t81 policy compile my_policy.apl
```

### `run`
Validates and loads a policy file to check for errors.

```bash
t81 policy run my_policy.apl
```

## 6.5 Trace Analysis (`trace`)

Tools for working with Axion execution traces.

### `show`
Visualizes a trace file with syntax highlighting.

```bash
t81 trace show trace.txt
```

### `diff`
Compares two trace files to identify divergence.

```bash
t81 trace diff trace_a.txt trace_b.txt
```

### `replay`
Replays a trace against a TISC binary to verify that the execution matches the log.

```bash
t81 trace replay program.tisc trace.txt
```

## 6.6 Sample Workflow

1.  **Write Code**: Create `math.t81`.
    ```t81
    fn main() -> T81Int { return 42; }
    ```
2.  **Check**: `t81 check math.t81`
3.  **Compile**: `t81 compile math.t81 -o math.tisc`
4.  **Inspect**: `t81 disasm math.tisc`
    ```asm
    ; Disassembly
    LOADI R1, 42
    RET
    ```
5.  **Run**: `t81 run math.tisc`
