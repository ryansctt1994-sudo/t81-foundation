#include <cassert>
#include <iostream>
#include <vector>
#include "t81/tisc/opcodes.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

// Define a simple test program
// 1. LoadImm R1, 10
// 2. LoadImm R2, "MyDesc"
// 3. ReflCap R3, R2 (Capture state into frame handle R3)
// 4. ReflTrace R3 (Add trace to frame R3)
// 5. LoadImm R4, "JustificationStep"
// 6. ReflJustify R3, R4 (Add justification step)
// 7. LoadImm R5, "MyDesc"
// 8. ReflCheck R6, R3, R5 (Check if "MyDesc" is in frame)
// 9. LoadImm R7, "NotInFrame"
// 10. ReflCheck R8, R3, R7 (Check if "NotInFrame" is in frame)
// 11. ReflSeal R3 (Seal the frame)
// 12. Halt

int main() {
  using namespace t81::tisc;
  Program prog;

  // Symbols
  prog.symbol_pool.push_back("MyDesc");
  prog.symbol_pool.push_back("JustificationStep");
  prog.symbol_pool.push_back("NotInFrame");

  // Instructions
  // LoadImm R1, 10
  prog.insns.push_back({Opcode::LoadImm, 1, 10, 0, LiteralKind::Int});

  // LoadImm R2, "MyDesc" (handle 1)
  prog.insns.push_back({Opcode::LoadImm, 2, 1, 0, LiteralKind::SymbolHandle});

  // ReflCap R3, R2
  prog.insns.push_back({Opcode::ReflCap, 3, 2, 0});

  // ReflTrace R3
  prog.insns.push_back({Opcode::ReflTrace, 3, 0, 0});

  // LoadImm R4, "JustificationStep" (handle 2)
  prog.insns.push_back({Opcode::LoadImm, 4, 2, 0, LiteralKind::SymbolHandle});

  // ReflJustify R3, R4
  prog.insns.push_back({Opcode::ReflJustify, 3, 4, 0});

  // Check "MyDesc"
  // LoadImm R5, "MyDesc" (handle 1)
  prog.insns.push_back({Opcode::LoadImm, 5, 1, 0, LiteralKind::SymbolHandle});
  // ReflCheck R6, R3, R5
  prog.insns.push_back({Opcode::ReflCheck, 6, 3, 5});

  // Check "NotInFrame"
  // LoadImm R7, "NotInFrame" (handle 3)
  prog.insns.push_back({Opcode::LoadImm, 7, 3, 0, LiteralKind::SymbolHandle});
  // ReflCheck R8, R3, R7
  prog.insns.push_back({Opcode::ReflCheck, 8, 3, 7});

  // ReflSeal R3
  prog.insns.push_back({Opcode::ReflSeal, 3, 0, 0});

  prog.insns.push_back({Opcode::Halt, 0, 0, 0});

  auto vm = t81::vm::make_interpreter_vm(nullptr);
  vm->load_program(prog);

  auto result = vm->run_to_halt(100);
  if (!result) {
    std::cerr << "VM Error: " << t81::vm::to_string(result.error()) << std::endl;
    return 1;
  }

  const auto& state = vm->state();

  // Verify R6 is true (1)
  if (state.registers[6] != 1) {
    std::cerr << "Test Failed: R6 should be 1, got " << state.registers[6] << std::endl;
    return 1;
  }

  // Verify R8 is false (0)
  if (state.registers[8] != 0) {
    std::cerr << "Test Failed: R8 should be 0, got " << state.registers[8] << std::endl;
    return 1;
  }

  std::cout << "Test Passed: ReflCheck, ReflTrace, ReflSeal executed correctly." << std::endl;
  return 0;
}
