#include <iostream>
#include <memory>
#include <vector>
#include "test_runtime_check.hpp"

#include "t81/cog/tier.hpp"
#include "t81/isa/program.hpp"
#include "t81/vm/vm.hpp"

using namespace t81;

int main() {
  auto vm = vm::make_interpreter_vm();

  t81::tisc::Program program;
  // R1: counter
  // R2: limit (250)
  // R3: step (1)
  // R4: function address (6)
  // R5: comparison result

  // 0: LoadImm R1, 0
  // 1: LoadImm R2, 250
  // 2: LoadImm R3, 1
  // 3: LoadImm R4, 6
  // 4: Call R4
  // 5: Halt
  // 6: Less R5, R1, R2
  // 7: JumpIfZero 11, R5 (Jump to 11 if R5 is 0)
  // 8: Add R1, R1, R3
  // 9: Call R4
  // 10: Ret
  // 11: Ret

  program.insns = {{t81::tisc::Opcode::LoadImm, 1, 0},
                   {t81::tisc::Opcode::LoadImm, 2, 250},
                   {t81::tisc::Opcode::LoadImm, 3, 1},
                   {t81::tisc::Opcode::LoadImm, 4, 6},
                   {t81::tisc::Opcode::Call, 0, 4},
                   {t81::tisc::Opcode::Halt},
                   // Function starts at 6
                   {t81::tisc::Opcode::Less, 5, 1, 2},
                   {t81::tisc::Opcode::JumpIfZero, 11, 5},
                   {t81::tisc::Opcode::Add, 1, 1, 3},
                   {t81::tisc::Opcode::Call, 0, 4},
                   {t81::tisc::Opcode::Ret},
                   {t81::tisc::Opcode::Ret}};

  vm->load_program(program);

  auto result = vm->run_to_halt(100000);

  if (!result.has_value()) {
    std::cerr << "VM trapped: " << static_cast<int>(result.error()) << "\n";
    return 1;
  }

  const auto& state = vm->state();

  std::cout << "Final call depth: " << state.contexts[0].call_depth << "\n";
  std::cout << "Final tier: " << static_cast<int>(state.contexts[0].tier_status.current) << "\n";

  // Check if we reached Tier 2
  if (state.contexts[0].tier_status.current != t81::cog::TierId::Tier2) {
    std::cerr << "Expected Tier 2 (" << static_cast<int>(t81::cog::TierId::Tier2) << "), got "
              << static_cast<int>(state.contexts[0].tier_status.current) << "\n";
    return 1;
  }

  return 0;
}
