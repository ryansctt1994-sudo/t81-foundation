# TISC Toolchain Sync Audit (v1.1.0)

**Date:** 2025-02-15
**Auditor:** Jules (Spec-Driven Verification Agent)

## 1. Objective
Verify that the TISC toolchain (Assembler, Disassembler, Pretty Printer, Emitter) recognizes the frozen v1.1.0 ISA, specifically the 7 new bitwise opcodes.

## 2. Audit Targets & Findings

### A. Pretty Printer (`core/isa/pretty_printer.cpp`)

*   **Status**: **FAIL**
*   **Finding**: The `opcode_to_string` function does not contain cases for the new bitwise opcodes:
    *   `BitAnd`, `BitOr`, `BitXor`, `BitNot`
    *   `BitShl`, `BitShr`, `BitUShr`
*   **Impact**: Debug output for these instructions will render as `"???"`.

### B. Binary Emitter (`core/isa/binary_emitter.cpp`)

*   **Status**: **FAIL**
*   **Finding**: The `map_opcode` function in `core/isa/binary_emitter.cpp` does not map the IR opcodes to the VM opcodes for:
    *   `BITAND`, `BITOR`, `BITXOR`, `BITNOT`
    *   `BITSHL`, `BITSHR`, `BITUSHR`
*   **Impact**: The toolchain cannot emit binary code for these instructions from IR.

### C. Assembler / Parser

*   **Status**: **Not Audited** (Files not present in `core/isa/` or `include/t81/isa/` during initial scan).
*   **Note**: If the assembler relies on `BinaryEmitter`, it is blocked.

## 3. Remediation Plan

The following tasks must be performed immediately after the freeze lock-in:

1.  **Update `pretty_printer.cpp`**: Add `case` statements for all 7 bitwise opcodes in `opcode_to_string`.
2.  **Update `binary_emitter.cpp`**: Add `case` statements in `map_opcode` to map IR constants to `t81::tisc::Opcode` enum values.
3.  **Update IR Definitions**: Ensure `include/t81/isa/ir.hpp` defines the corresponding IR opcodes if missing.

## 4. Conclusion

The toolchain is currently **out of sync** with the VM and Spec. While the VM can execute the opcodes if valid binary is provided, the current C++ toolchain libraries cannot generate or pretty-print them. This does not invalidate the ISA freeze but requires immediate engineering follow-up.
