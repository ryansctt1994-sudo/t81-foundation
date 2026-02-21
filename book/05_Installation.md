# Chapter 5: Installation and Build Verification

## 5.1 Prerequisites

The T81 reference implementation is written in C++23. To build the Sovereign Compute stack, the following tools are required:

| Component | Minimum Version | Reason |
| :--- | :--- | :--- |
| **Compiler** | GCC 14+ or Clang 18+ | C++23 features (`std::expected`, `std::print`) |
| **CMake** | 3.25+ | Build configuration |
| **Python** | 3.10+ | Testing and binding generation |
| **Pybind11** | 2.10+ | Python bindings (`t81_python`) |

## 5.2 Building from Source

### 5.2.1 Standard Release Build
This build profile optimizes for performance on the host architecture. It typically corresponds to **Tier C (Host-Tolerant)** determinism, using hardware floating-point instructions.

```bash
git clone https://github.com/t81dev/t81-foundation.git
cd t81-foundation
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)
```

### 5.2.2 Debug / Audit Build
For development and auditing, use the Debug profile with sanitizers enabled.

```bash
cmake -S . -B build_debug \
    -DCMAKE_BUILD_TYPE=Debug \
    -DT81_ENABLE_SANITIZERS=ON \
    -DT81_BUILD_TESTS=ON
cmake --build build_debug
```

## 5.3 Verifying the Build

After compilation, it is mandatory to verify the correctness of the binary before using it for sovereign tasks.

### 5.3.1 The Unit Test Suite
The standard test suite covers VM opcodes, data type invariants, and serialization rules.

```bash
cd build
ctest --output-on-failure
```

Key tests to watch:
*   `t81_property_invariants_test`: Verifies mathematical properties of `T81Int` and `T81Fraction`.
*   `t81_ethics_test`: Verifies Axion policy enforcement.

### 5.3.2 The Determinism Gate
The `t81lang_repro_gate.py` script performs an end-to-end verification of the compiler and VM. It compiles a reference suite of T81Lang programs and compares the resulting TISC bytecode and execution traces against canonical artifacts.

```bash
# Run the gate
python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
```

**Failure Criteria**:
*   Any bit-difference in `.tisc` output.
*   Any divergence in the Axion execution trace.

### 5.3.3 Floating Point Profile Check
To verify which floating-point backend is active (Hardware vs. Software/dmath), run the float property test:

```bash
./build/t81_property_float_test
```
If this test fails on transcendental functions, the build is not compliant with the **Strict Determinism Profile (Tier A)**.

## 5.4 Python Bindings

To install the Python bindings for embedding T81VM in a Python workflow:

```bash
pip install .
```
Or manually:
```bash
cmake -S . -B build -DT81_BUILD_PYTHON=ON
cmake --build build --target t81_python
```
The module `t81` will be available in `build/lib`.

## 5.5 Docker Environment

For a guaranteed reference environment, use the official Docker image. This image is pinned to specific versions of Clang and libc to minimize environment noise.

```bash
docker build -t t81-sovereign .
docker run -it t81-sovereign t81 check examples/hello_world.t81
```
