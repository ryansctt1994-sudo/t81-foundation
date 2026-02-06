#pragma once
#include <cstdint>

namespace t81::ir {

/**
 * @enum Opcode
 * @brief Canonical T81 IR opcodes, unifying TISC and legacy NewBook ISA.
 *
 * Values are stable; extend by appending to specific blocks.
 */
enum class Opcode : uint16_t {
  // --- Meta / Control (0x00xx) ---
  Nop        = 0x0000,
  Halt       = 0x0001,
  Jump       = 0x0002,
  JumpIfZero = 0x0003,
  JumpIfNotZero = 0x0004,
  JumpIfNeg  = 0x0005,
  JumpIfPos  = 0x0006,
  Call       = 0x0007,
  Ret        = 0x0008,
  Trap       = 0x0009,

  // --- Integer / Scalar ALU (0x01xx) ---
  Add        = 0x0100,
  Sub        = 0x0101,
  Mul        = 0x0102,
  Div        = 0x0103,
  Mod        = 0x0104,
  Rem        = 0x0105,
  And        = 0x0106,
  Or         = 0x0107,
  Xor        = 0x0108,
  Not        = 0x0109,
  Neg        = 0x010A,
  Inc        = 0x010B,
  Dec        = 0x010C,
  Cmp        = 0x010D,
  Move       = 0x010E,
  LoadImm    = 0x010F,

  // --- BigInt Ops (T243) (0x02xx) ---
  BigAdd     = 0x0200,
  BigSub     = 0x0201,
  BigMul     = 0x0202,
  BigDiv     = 0x0203,
  BigMod     = 0x0204,
  BigCmp     = 0x0205,

  // --- Tensor Ops (T729) (0x03xx) ---
  TDot       = 0x0300,
  TTranspose = 0x0301,
  TSlice2D   = 0x0302,
  TReshape   = 0x0303,
  TMatMul    = 0x0304,
  TReduce    = 0x0305,
  TVecAdd    = 0x0306,

  // --- Memory / Stack / IO (0x04xx) ---
  Load       = 0x0400,
  Store      = 0x0401,
  Push       = 0x0402,
  Pop        = 0x0403,
  StackAlloc = 0x0404,
  StackFree  = 0x0405,
  HeapAlloc  = 0x0406,
  HeapFree   = 0x0407,

  // --- Axion / Capability / System (0x05xx) ---
  AxRead     = 0x0500,
  AxSet      = 0x0501,
  AxVerify   = 0x0502,
  CapCheck   = 0x0503,
  CapGrant   = 0x0504,
  WeightsLoad = 0x0505,
};

/**
 * @enum OpcodeFlags
 * @brief Metadata flags for IR opcodes.
 */
enum OpcodeFlags : uint32_t {
  OP_FLAG_NONE       = 0,
  OP_FLAG_PRIVILEGED = 1 << 0, ///< Requires Axion privileged context.
  OP_FLAG_BRANCH     = 1 << 1, ///< Instruction may change PC non-sequentially.
  OP_FLAG_TERMINATOR = 1 << 2, ///< Ends a basic block.
  OP_FLAG_MEMORY     = 1 << 3, ///< Accesses memory (Load/Store).
};

/**
 * @struct OpcodeDesc
 * @brief Metadata description for an opcode.
 */
struct OpcodeDesc {
  Opcode op;
  const char* name;
  uint32_t flags;
};

inline OpcodeDesc get_opcode_desc(Opcode op) {
  switch (op) {
    case Opcode::Halt: return {op, "halt", OP_FLAG_TERMINATOR};
    case Opcode::Jump: return {op, "jump", OP_FLAG_BRANCH | OP_FLAG_TERMINATOR};
    case Opcode::AxRead:
    case Opcode::AxSet:
    case Opcode::AxVerify:
    case Opcode::CapGrant: return {op, "axion_op", OP_FLAG_PRIVILEGED};
    case Opcode::Load:
    case Opcode::Store: return {op, "mem_op", OP_FLAG_MEMORY};
    default: return {op, "unknown", OP_FLAG_NONE};
  }
}

} // namespace t81::ir
