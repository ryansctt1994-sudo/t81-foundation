#include <cassert>
#include <iostream>
#include <vector>
#include "t81/tisc/opcodes.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

// Test Tier 3 Opcodes: Recurse, Contract, Entropy, Depth, Terminate

int main() {
  using namespace t81::tisc;
  Program prog;

  // Instructions
  // 1. Check initial Depth into R1
  prog.insns.push_back({Opcode::Depth, 1, 0, 0});

  // 2. Recurse (Depth -> 1)
  prog.insns.push_back({Opcode::Recurse, 0, 0, 0});

  // 3. Check Depth into R2
  prog.insns.push_back({Opcode::Depth, 2, 0, 0});

  // 4. Recurse (Depth -> 2)
  prog.insns.push_back({Opcode::Recurse, 0, 0, 0});

  // 5. Check Depth into R3
  prog.insns.push_back({Opcode::Depth, 3, 0, 0});

  // 6. Contract (entropy=0)
  // Need to put entropy in register first.
  prog.insns.push_back({Opcode::LoadImm, 10, 0, 0, LiteralKind::Int});
  prog.insns.push_back({Opcode::Contract, 0, 10, 0});

  // 7. Entropy into R4
  prog.insns.push_back({Opcode::Entropy, 4, 0, 0});

  // 8. Terminate (Depth -> 1)
  prog.insns.push_back({Opcode::Terminate, 0, 0, 0});

  // 9. Check Depth into R5
  prog.insns.push_back({Opcode::Depth, 5, 0, 0});

  // 10. Terminate (Depth -> 0)
  prog.insns.push_back({Opcode::Terminate, 0, 0, 0});

  // 11. Check Depth into R6
  prog.insns.push_back({Opcode::Depth, 6, 0, 0});

  // 12. Halt
  prog.insns.push_back({Opcode::Halt, 0, 0, 0});

  auto vm = t81::vm::make_interpreter_vm(nullptr);
  vm->load_program(prog);

  auto result = vm->run_to_halt(100);
  if (!result) {
    std::cerr << "VM Error: " << t81::vm::to_string(result.error()) << std::endl;
    return 1;
  }

  const auto& state = vm->state();

  bool failed = false;

  // R1: Initial Depth = 0
  if (state.contexts[0].registers[1] != 0) {
    std::cerr << "Test Failed: R1 (Initial Depth) should be 0, got " << state.contexts[0].registers[1] << "\n";
    failed = true;
  }

  // R2: Depth after 1st Recurse = 1
  if (state.contexts[0].registers[2] != 1) {
    std::cerr << "Test Failed: R2 (Depth after 1st Recurse) should be 1, got " << state.contexts[0].registers[2]
              << "\n";
    failed = true;
  }

  // R3: Depth after 2nd Recurse = 2
  if (state.contexts[0].registers[3] != 2) {
    std::cerr << "Test Failed: R3 (Depth after 2nd Recurse) should be 2, got " << state.contexts[0].registers[3]
              << "\n";
    failed = true;
  }

  // R5: Depth after 1st Terminate = 1
  if (state.contexts[0].registers[5] != 1) {
    std::cerr << "Test Failed: R5 (Depth after 1st Terminate) should be 1, got "
              << state.contexts[0].registers[5] << "\n";
    failed = true;
  }

  // R6: Depth after 2nd Terminate = 0
  if (state.contexts[0].registers[6] != 0) {
    std::cerr << "Test Failed: R6 (Depth after 2nd Terminate) should be 0, got "
              << state.contexts[0].registers[6] << "\n";
    failed = true;
  }

  if (failed) {
    return 1;
  }

  std::cout << "Test Passed: Tier 3 opcodes (Recurse, Contract, Entropy, Depth, Terminate) "
            << "executed correctly." << std::endl;
  return 0;
}
