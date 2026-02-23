# Chapter 6: CLI and API Usage

## 6.1 The T81 Command Line Interface

**Status: Implemented**

The `t81` binary is the primary entry point for interaction with the runtime. It follows the standard `subcommand` pattern.

### 6.1.1 Compilation (`compile`)
Compiles T81 source code (`.t81`) into TISC bytecode (`.tisc`).

```bash
t81 compile main.t81 -o main.tisc
```

**Options**:
*   `-o, --output <file>`: Output bytecode path. Defaults to `a.tisc`.
*   `--policy <file>`: Attach an Axion policy to the bytecode (embedded). This policy will be enforced at runtime.
*   `--optimize <level>`: Optimization level (0=None, 1=Basic, 2=Aggressive). Default 1.
*   `--dump-ast`: Print the canonical AST to stdout (debug).

### 6.1.2 Execution (`run`)
Executes a TISC bytecode file or a source file (JIT-compile-and-go).

```bash
# Run bytecode
t81 run main.tisc

# Run source directly
t81 run main.t81
```

**Options**:
*   `--policy <file>`: Enforce a specific Axion policy file during execution. Overrides embedded policy if stricter.
*   `--weights <file>`: Attach a tensor model (`.t81w`, `.safetensors`, `.gguf`) to the context. Can be repeated for multiple models.
*   `--trace <file>`: Output the Axion Audit Trace to a file.
*   `--gas <int>`: Set a hard limit on instruction cycles.

### 6.1.3 Trace Analysis (`trace`)
The `trace` subcommand suite manages the Axion audit logs.

*   `t81 trace show <trace_file>`: Human-readable dump of a trace. Shows opcodes, verdicts, and hash chains.
*   `t81 trace verify <trace_file>`: Cryptographically verify the Merkle chain integrity.
*   `t81 trace diff <trace_a> <trace_b>`: Compare two traces for divergence. Highlights the first differing tick.
*   `t81 trace replay <program.tisc> <trace_file>`: Re-execute a program and verify it produces the exact same trace as the file.

### 6.1.4 Interactive Mode (`repl`)
Launches the Read-Eval-Print Loop.

```bash
t81 repl
```
Commands inside REPL:
*   `:load <file>`: Load a script.
*   `:model <path>`: Load a weights file dynamically.
*   `:trace`: Show the trace of the last execution.
*   `:tier <n>`: Request promotion to Tier N.
*   `:quit`: Exit.

## 6.2 Embedding T81 (C++ API)

**Status: Stable**

To embed T81 into a host application (e.g., a game engine or a distributed node), use `t81::vm::IVirtualMachine`.

```cpp
#include <t81/vm/vm.hpp>
#include <t81/tisc/program.hpp>
#include <t81/axion/policy.hpp>

int main() {
    // 1. Create VM with default configuration
    auto vm = t81::vm::make_interpreter_vm();

    // 2. Load Program from disk
    auto prog_result = t81::tisc::load_program("main.tisc");
    if (!prog_result) { /* handle error */ }
    vm->load_program(prog_result.value());

    // 3. Configure Policy (e.g., limit recursion to 100)
    t81::axion::Policy policy;
    policy.max_recursion_depth = 100;
    vm->set_policy(policy);

    // 4. Run
    auto result = vm->run_to_halt();
    if (!result) {
        std::cerr << "Trapped: " << result.error().reason << "\n";
        return 1;
    }

    // 5. Inspect Result
    std::cout << "Result: " << vm->get_result_register() << "\n";
    return 0;
}
```

## 6.3 Embedding T81 (Python API)

**Status: Implemented**

The Python bindings allow driving the T81VM from Python, primarily for testing and orchestration.

```python
import t81

# Create VM
vm = t81.VirtualMachine()

# Load Code
vm.load_source("""
let x = 10;
let y = 20;
x + y;
""")

# Execute
try:
    result = vm.run()
    print(f"Result: {result}") # Output: 30

    # Access the trace
    trace = vm.get_trace()
    print(f"Hash: {trace.final_hash}")
except t81.SecurityFault as e:
    print(f"Policy Violation: {e}")
```

## 6.4 Debugging

**Status: Implemented**

The `t81 debug` command launches a GDB-style debugger for TISC.

*   `step` / `s`: Step one instruction.
*   `next` / `n`: Step over call.
*   `reg`: Dump registers.
*   `stack`: Dump stack.
*   `mem <segment> <offset>`: Inspect memory.
*   `trace`: Show recent trace history.
*   `break <pc>`: Set breakpoint.

```bash
t81 debug main.tisc
(t81-gdb) break 10
(t81-gdb) run
Breakpoint at PC=10
(t81-gdb) reg r1
r1 = 42 (Int) [Tag: Int]
(t81-gdb) stack
SP -> [ 10 ]
      [ 0 ]
```
