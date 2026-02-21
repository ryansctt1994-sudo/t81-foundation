# Chapter 4: Implementation Details

This chapter explores the codebase structure, build system, and engineering choices of the T81 Foundation project.

## 4.1 Technologies and Languages

The project is predominantly written in **C++** (Standards C++20 and C++23), chosen for its performance, control over memory layout, and template metaprogramming capabilities.

*   **C++ (93%)**: The core VM, Compiler, Axion Engine, and Tools.
*   **Python (4%)**: Used for CI scripts (`t81lang_repro_gate.py`), test fixtures, and Python bindings (`t81_python`).
*   **CMake (2%)**: The cross-platform build system generator.
*   **Shell**: Helper scripts for environment setup.

## 4.2 Repository Structure

The repository is organized as follows:

*   **`src/`**: The source code.
    *   **`core/`**: Fundamental types (`bigint.cpp`, `fraction.cpp`, `float.cpp`).
    *   **`vm/`**: The Virtual Machine implementation (`vm.cpp`, `jit_compiler.cpp`).
    *   **`tisc/`**: TISC bytecode handling (`binary_emitter.cpp`, `binary_io.cpp`).
    *   **`frontend/`**: The T81Lang compiler (`lexer.cpp`, `parser.cpp`, `semantic_analyzer.cpp`, `ir_generator.cpp`).
    *   **`axion/`**: The policy engine (`engine.cpp`, `policy_engine.cpp`).
    *   **`cli/`**: The command-line driver (`main.cpp`, `driver.cpp`).
    *   **`canonfs/`**: The filesystem implementation (`in_memory_driver.cpp`).
    *   **`tools/`**: Utilities for weights and models (`weights.cpp`).
    *   **`cog/`**: Cognitive Tier logic.

*   **`include/t81/`**: Public header files matching the source structure.
*   **`spec/`**: Normative markdown specifications (`tisc-spec.md`, `t81vm-spec.md`, etc.).
*   **`tests/`**: Comprehensive test suites.
    *   **`cpp/`**: Unit tests for C++ components (using CTest).
    *   **`fixtures/`**: T81Lang source files for integration testing.
*   **`scripts/`**: CI and utility scripts.
*   **`examples/`**: Sample T81Lang programs.

## 4.3 Build System

The project uses **CMake** (minimum version 3.16) as its primary build system.

### `CMakeLists.txt`
The root `CMakeLists.txt` defines the project and its options:
*   `T81_BUILD_TESTS`: Enables unit tests (Default: ON).
*   `T81_ENABLE_ASAN`: Enables AddressSanitizer (Default: OFF).
*   `T81_USE_CXX23`: Toggles C++23 mode (Default: ON, falls back to C++20).

It defines several libraries:
*   `t81_core`: The main static library containing the VM, Types, and Axion.
*   `t81_frontend`: The compiler frontend.
*   `t81_tisc`: TISC handling.
*   `t81_cli_driver`: The logic for the CLI.

And the main executable:
*   `t81`: Linked against the above libraries.

### Bazel Support
A `BUILD.bazel` file is provided for users who prefer the Bazel build system, commonly used in large-scale monorepos.

## 4.4 Supported Platforms

The project explicitly supports and verifies the following platforms via CI:

1.  **Linux (x86_64)**
    *   Compiler: Clang 18+, GCC 14+.
    *   Status: Primary development platform.
2.  **Linux (ARM64)**
    *   Compiler: Clang 18+.
    *   Status: Verified for bit-exact parity with x86_64.
3.  **macOS (ARM64 / Apple Silicon)**
    *   Compiler: Apple Clang.
    *   Status: Fully supported.

**Windows (MSVC)** support is experimental but included in the CMake configuration with specific compiler flags (`/W4`, `/O2`).

## 4.5 Tooling and Quality Assurance

*   **Clang-Format**: Enforces code style (Google style with modifications). Version 18 is required.
*   **Clang-Tidy**: Static analysis to catch bugs and modernize code.
*   **Pre-commit Hooks**: A `.pre-commit-config.yaml` file ensures that commits meet quality standards before being pushed.
*   **CI (GitHub Actions)**:
    *   Runs unit tests.
    *   Runs the **Determinism Gate** (`scripts/ci/t81lang_repro_gate.py`), which compiles fixture files twice and compares the binary output to ensure the compiler itself is deterministic.
