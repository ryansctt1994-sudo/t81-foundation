# Chapter 6: Usage and CLI

## 6.1 The Unified CLI

**Status: Implemented & Tested**

The `t81` executable provides a unified interface for all operations.

### 6.1.1 Basic Commands

*   **`compile`**: Compiles T81Lang source (`.t81`) to TISC bytecode (`.tisc`).
    ```bash
    t81 compile examples/hello_world.t81 -o hello.tisc
    ```

*   **`run`**: Executes TISC bytecode on the VM.
    ```bash
    t81 run hello.tisc
    ```

*   **`check`**: Performs syntax and semantic analysis without generating code.
    ```bash
    t81 check examples/hello_world.t81
    ```

### 6.1.2 Debugging and Inspection

*   **`disasm`**: Disassembles TISC bytecode into readable mnemonics.
    ```bash
    t81 disasm hello.tisc
    ```

*   **`debug`**: Launches the interactive debugger (step, inspect registers).
    ```bash
    t81 debug hello.tisc
    ```

*   **`trace`**: Manages Axion audit traces.
    ```bash
    t81 trace show trace.txt
    t81 trace diff trace_a.txt trace_b.txt
    t81 trace replay hello.tisc trace.txt
    ```

### 6.1.3 Model Management

*   **`weights`**: Tools for importing and quantizing neural network weights.
    ```bash
    t81 weights import model.safetensors -o model.t81w
    t81 weights info model.t81w
    t81 weights quantize model.safetensors --to-gguf model.gguf
    ```

*   **`canonize-tensor`**: Verifies and normalizes a tensor file.
    ```bash
    t81 canonize-tensor model.t81w
    ```

*   **`repro-hash`**: Computes the canonical hash of a directory for verification.
    ```bash
    t81 repro-hash tests/fixtures/t81lang_determinism
    ```

> **Verification**: Run `build/t81 --help` to see the exact current usage.
