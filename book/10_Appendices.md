# Chapter 10: Appendices

## 10.1 Glossary

*   **Axion**: The safety kernel and policy enforcement engine of T81.
*   **Balanced Ternary**: A numeral system using digits -1, 0, +1.
*   **CanonFS**: The Canonical File System; immutable and content-addressed.
*   **CanonHash-81**: The standard hash function used for content addressing (currently based on BLAKE3 mapped to Base-81).
*   **dmath**: The deterministic software math library.
*   **T81Lang**: The high-level programming language for T81.
*   **T81VM**: The Virtual Machine.
*   **TISC**: Ternary Instruction Set Computer (bytecode).
*   **Trit**: A ternary digit.
*   **Tryte**: A sequence of trits (usually 4 or 5), the basic addressable unit.

## 10.2 File Listing (Key Files)

### Source
*   `src/cli/main.cpp`: CLI Entry point.
*   `src/vm/vm.cpp`: Virtual Machine logic.
*   `src/axion/policy_engine.cpp`: Policy logic.
*   `src/tools/weights.cpp`: Weight management tools.

### Specifications
*   `spec/t81-data-types.md`
*   `spec/tisc-spec.md`
*   `spec/t81vm-spec.md`
*   `spec/axion-kernel.md`
*   `spec/canonfs-spec.md`
*   `spec/cognitive-tiers.md`
*   `spec/determinism-profile.md`
*   `spec/constitution.md`

### Build & CI
*   `CMakeLists.txt`: Build configuration.
*   `scripts/ci/t81lang_repro_gate.py`: Determinism verification script.
