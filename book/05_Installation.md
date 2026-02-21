# Chapter 5: Installation and Setup

This chapter guides you through setting up the T81 development environment, building the project from source, and verifying the installation.

## 5.1 Prerequisites

Before you begin, ensure your system meets the following requirements:

*   **Operating System**: Linux (modern distribution like Ubuntu 22.04+ or Fedora) or macOS (12+).
*   **C++ Compiler**: Must support C++20. Recommended: Clang 18+ or GCC 14+. Apple Clang on macOS.
*   **CMake**: Version 3.16 or higher.
*   **Python**: Version 3.8+ (for scripts and bindings).
*   **Git**: For version control.

### Optional Dependencies
*   **Ninja**: Recommended for faster builds (`sudo apt install ninja-build`).
*   **pybind11**: Required if you want to build Python bindings (usually fetched automatically or installed via pip).

## 5.2 Cloning the Repository

Clone the official repository using Git:

```bash
git clone https://github.com/t81dev/t81-foundation.git
cd t81-foundation
```

## 5.3 Building from Source

T81 uses CMake. The standard build process is:

1.  **Configure**: Create a build directory and configure the project.
    ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    ```
    *   `-S .`: Source directory is current directory.
    *   `-B build`: Build artifacts go into `build/`.
    *   `-DCMAKE_BUILD_TYPE=Release`: Optimized build. Use `Debug` for development.

2.  **Build**: Compile the code.
    ```bash
    cmake --build build --parallel
    ```
    *   `--parallel`: Uses all available CPU cores.

## 5.4 Verification

After a successful build, you should verify that the system is working correctly and deterministically.

### 1. Run the "Hello World" Test
Compile and run a simple T81Lang program:

```bash
./build/t81 compile examples/hello_world.t81 -o hello.tisc
./build/t81 run hello.tisc
```

You should see the output of the program (e.g., a calculation result).

### 2. Run the Determinism Gate
This script ensures that your build of the compiler is deterministic (i.e., it produces identical bytecode on repeated runs).

```bash
python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
```

If this passes, your toolchain is sound.

### 3. Run Unit Tests
Execute the full test suite to verify all components:

```bash
cd build
ctest --output-on-failure
```

## 5.5 Quick Start Demos

The build includes several standalone demos.

*   **VM Demo**: Runs a hardcoded TISC sequence directly in the VM.
    ```bash
    ./build/t81_demo
    ```

*   **Axion Demo**: Demonstrates the policy engine enforcing rules.
    ```bash
    ./build/axion_demo
    ```

## 5.6 Troubleshooting

*   **"CMake Error: C++ compiler does not support C++20"**: Upgrade your compiler (Clang or GCC).
*   **"pybind11 not found"**: Ensure you have internet access so CMake can fetch dependencies, or install `pybind11-dev`.
*   **Linker errors**: If you are using a custom toolchain, ensure `libc++` or `libstdc++` are correctly linked.
