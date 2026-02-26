# Chapter 6: CLI and API Usage

## 6.1 The T81 Command Line Interface

**Status: Implemented**

The `t81` binary is the main operator entry point for compile/run/debug and governance workflows.

Core commands:
* `compile`, `run`, `check`, `lint`, `disasm`, `debug`
* `repro-hash`
* `weights`, `policy`, `trace`
* `canonize-tensor`, `canonize-file`
* `llama-run` (experimental, non-DCP)

### 6.1.1 Compilation (`compile`)
Compiles T81 source code (`.t81`) into TISC bytecode (`.tisc`).

```bash
t81 compile main.t81 -o main.tisc
```

**Options**:
*   `-o, --output <file>`: Output bytecode path. Defaults to `a.tisc`.
*   `--weights-model <model.t81w>`: Attach a native weights model at compile time.

### 6.1.2 Execution (`run`)
Executes a TISC bytecode file or source file (compile-then-run).

```bash
# Run bytecode
t81 run main.tisc

# Run source directly
t81 run main.t81
```

**Options**:
*   `--policy <file.apl>`: Apply Axion policy.
*   `--trace`: Enable execution trace output.
*   `--weights-model <model.t81w>`: Attach a native weights model.

### 6.1.3 Trace Analysis (`trace`)
The `trace` subcommand suite manages Axion trace artifacts.

*   `t81 trace show <trace_file>`: Human-readable view of a trace.
*   `t81 trace diff <trace_a> <trace_b>`: Compare two traces.
*   `t81 trace replay <program.tisc> <trace_file>`: Re-execute and verify trace match.

### 6.1.4 Interactive Mode (`repl`)
Launches the Read-Eval-Print Loop.

```bash
t81 repl
```

Common REPL commands:
*   `:load <file>`: Load a script.
*   `:model <path>`: Show/replace attached weights model.
*   `:history`: Show snippet history.
*   `:help`: Show REPL commands.
*   `:quit`: Exit.

### 6.1.5 Governance and Model Utilities

```bash
# Determinism fixture hash gate
t81 repro-hash

# Canonize arbitrary bytes to CanonFS
t81 canonize-file ./model.gguf

# Policy tooling
t81 policy compile policy.apl -o policy.axionb
t81 policy run policy.apl

# Weights tooling
t81 weights import model.safetensors -o model.t81w
t81 weights info model.t81w
t81 weights quantize model.safetensors --to-gguf model.gguf
```

Governed inference path (experimental, non-DCP):

```bash
t81 llama-run ./model.gguf "hello" --policy policy.apl --max-tokens 32
```

### 6.1.6 Governed `llama-run` Boundary

`llama-run` is explicitly **experimental, non-DCP**.

Operational constraints:

1. `--policy <policy.apl>` is required.
2. Model authorization should be hash-bound (for example via policy and expected hash controls).
3. Reproducibility evidence for this surface is governance-facing and does not imply DCP certification.

## 6.2 Embedding T81 (C++ API)

**Status: Stable Public Headers**

To embed T81 into a host application, use `t81::vm::IVirtualMachine`.

```cpp
#include <t81/vm/vm.hpp>
#include <t81/axion/engine.hpp>

int main() {
  auto vm = t81::vm::make_interpreter_vm();
  t81::tisc::Program p;
  p.insns.push_back({t81::tisc::Opcode::Halt, 0, 0, 0});
  vm->load_program(p);
  auto rc = vm->run_to_halt();
  return rc ? 0 : 1;
}
```

## 6.3 Embedding T81 (Python API)

**Status: Implemented (testing/orchestration path)**

Python bindings are exposed as `t81_python` in the build output.

```python
import t81_python

source = "fn main() -> i32 { return 1 + 2; }"
program = t81_python.compile(source)
vm = t81_python.make_interpreter_vm()
vm.load_program(program)
vm.run_to_halt()
print(vm.get_register(0))
```

## 6.4 Debugging

**Status: Implemented**

The `t81 debug` command launches an interactive debugger for TISC.

*   `step` / `s`: Step one instruction.
*   `next` / `n`: Step over call.
*   `reg`: Dump registers.
*   `stack`: Dump stack.
*   `mem <segment> <offset>`: Inspect memory.
*   `break <pc>`: Set breakpoint.

```bash
t81 debug main.tisc
```

<!-- chapter-nav-start -->

---

**Navigation**

- [Book Index](./README.md)
- [Previous: Chapter 5: Installation and Build Verification](./05_Installation.md)
- [Next: Chapter 7: Programming in T81Lang](./07_Programming_in_T81Lang.md)

<!-- chapter-nav-end -->
