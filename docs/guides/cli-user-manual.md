# T81 CLI User Manual

This is the operator manual for the `t81` CLI.
Everything in this document is intended to match the current shipped binary behavior.

**Last Updated:** February 26, 2026

## 1. Quick Start

Build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Show top-level help:

```bash
./build/t81 --help # docs-smoke
```

Minimal compile/run flow:

```bash
./build/t81 check examples/hello_world.t81
./build/t81 compile examples/hello_world.t81 -o hello.tisc
./build/t81 run hello.tisc
```

## 2. Global Options

These options are accepted before or after the command token:

- `-h`, `--help`
- `-V`, `--version`
- `-q`, `--quiet`
- `-v`, `--verbose`

Examples:

```bash
./build/t81 -q version # docs-smoke
./build/t81 version -q # docs-smoke
./build/t81 compile --help # docs-smoke
./build/t81 help compile # docs-smoke
./build/t81 help advanced # docs-smoke
./build/t81 help labs # docs-smoke
```

## 3. Command Tiers

`t81 --help` shows the core workflow commands.
Additional command groups are discoverable via:

- `t81 help advanced`
- `t81 help labs`

## 4. Commands

### 4.1 `compile`

```text
t81 compile <file.t81|file.t81w> [-o <file.tisc>] [--weights-model <model.t81w>]
```

Compiles source into TISC bytecode.

### 4.2 `run`

```text
t81 run <file.t81|file.tisc> [--policy <policy.apl>] [--trace] [--weights-model <model.t81w>]
```

Compiles if needed and executes via VM.

### 4.3 `disasm`

```text
t81 disasm <file.tisc>
```

Prints human-readable disassembly.

### 4.4 `debug`

```text
t81 debug <file.t81|file.tisc> [--policy <policy.apl>] [--weights-model <model.t81w>]
```

Compiles if needed and starts debugger.

### 4.5 `check` / `lint`

```text
t81 check <file.t81>
t81 lint <file.t81>
```

Syntax + semantic validation without bytecode emission.

### 4.6 `repl`

```text
t81 repl [--weights-model <model.t81w>] [--policy <policy.apl>]
```

Starts the interactive REPL.

### 4.7 `repro-hash`

```text
t81 repro-hash [fixtures_dir]
```

Runs the T81Lang reproducibility fixture hash gate.

### 4.8 `canonize-tensor`

```text
t81 canonize-tensor <file>
```

Canonicalizes tensor input into CanonFS object storage.

### 4.9 `canonize-file`

```text
t81 canonize-file <file> [--canonfs-root <path>]
```

Writes raw file bytes to CanonFS and prints `sha3-256:<hash>`.

### 4.10 `init`

```text
t81 init <project_name>
```

Creates a project directory with `main.t81` and `README.md`.

### 4.11 `pkg`

```text
t81 pkg <subcommand> [args]
t81 pkg init [package_name]
t81 pkg check
```

Creates/validates `package.t81`.

### 4.12 `benchmark`

```text
t81 benchmark [benchmark_runner_flags...]
```

Runs benchmark runner with forwarded benchmark flags.

### 4.13 `weights`

```text
t81 weights <subcommand> [options]
t81 weights import <file> [-o <out>] [--format <fmt>]
t81 weights info <model.t81w> [--json]
t81 weights quantize <input> --to-gguf <out>
```

Weight import/info/quantization helpers.

### 4.14 `policy`

```text
t81 policy <subcommand> [options]
t81 policy compile <file.apl> [-o <out>]
t81 policy run <file.apl|file.axionb> [--json]
```

Policy compile/validation helpers.

### 4.15 `trace`

```text
t81 trace <subcommand> [args]
t81 trace show <trace.txt> [--no-color]
t81 trace diff <trace1.txt> <trace2.txt> [--no-color]
t81 trace replay <file.tisc> <trace.txt>
t81 trace export <trace.txt> [--format <json|csv>] [-o <file>]
```

Trace inspection and export utilities.

### 4.16 `llama-run` (experimental)

```text
t81 llama-run <model.gguf|sha3-256:hash> <prompt> --policy <policy.apl> [options]
```

Options:

- `--max-tokens <n>`
- `--seed <n>`
- `--threads <n>`
- `--temperature <x>`
- `--top-k <n>`
- `--top-p <x>`
- `--expected-model-hash <h>`
- `--canonfs-root <path>`

## 5. Help Contract

Supported help forms:

```bash
./build/t81 --help # docs-smoke
./build/t81 help # docs-smoke
./build/t81 help compile # docs-smoke
./build/t81 compile --help # docs-smoke
./build/t81 help advanced # docs-smoke
./build/t81 help labs # docs-smoke
```

Unknown help topics return non-zero.

## 6. Exit and Output Behavior

- `0` on success.
- non-zero on failure.
- command result output is written to `stdout`.
- diagnostics/errors are written to `stderr` and prefixed with `error:`.

Runtime trap exit codes used by `t81 run` / `t81 debug`:

| Code | Meaning |
| :--- | :--- |
| `10` | Division fault |
| `12` | Bounds fault |
| `13` | Security fault |
| `14` | Decode fault |
| `15` | Explicit trap instruction |
| `16` | Type fault |
| `17` | Stack fault |
| `18` | Shape fault |
| `19` | Tier fault |

## 7. Known Command Validation Rules

- `check` / `lint` require `.t81`.
- `disasm` requires `.tisc`.
- `run` and `debug` require `.t81` or `.tisc`.
- `compile` accepts `.t81` and `.t81w`.
- `pkg init` enforces package name characters: alphanumeric, `_`, `-`.

## 8. Notes on Documentation Scope

This manual intentionally excludes deprecated or non-shipping flags.
If behavior changes, update this file in the same change set as the CLI implementation.
