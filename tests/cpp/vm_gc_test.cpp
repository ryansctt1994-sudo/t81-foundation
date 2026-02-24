#include <iostream>
#include <vector>
#include "t81/isa/opcodes.hpp"
#include "t81/isa/program.hpp"
#include "t81/vm/vm.hpp"

int main() {
  auto vm = t81::vm::make_interpreter_vm(nullptr);
  t81::tisc::Program prog;

  // 0: LoadImm R2, 2000 (Counter)
  // 1: LoadImm R3, 10 (Size)
  // 2: TNew R1, R3  <-- Loop start
  // 3: Dec R2
  // 4: JumpIfNotZero R2, 2 (Jump to address 2)
  // 5: Halt

  prog.insns.push_back({t81::tisc::Opcode::LoadImm, 2, 2000, 0, t81::tisc::LiteralKind::Int});
  prog.insns.push_back({t81::tisc::Opcode::LoadImm, 3, 10, 0, t81::tisc::LiteralKind::Int});
  prog.insns.push_back({t81::tisc::Opcode::TNew, 1, 3});
  prog.insns.push_back({t81::tisc::Opcode::Dec, 2});
  prog.insns.push_back({t81::tisc::Opcode::JumpIfNotZero, 2, 2});
  prog.insns.push_back({t81::tisc::Opcode::Halt});

  vm->load_program(prog);

  // Run
  auto res = vm->run_to_halt(20000);  // Enough steps
  if (!res) {
    std::cerr << "VM Error: " << static_cast<int>(res.error()) << "\n";
    return 1;
  }

  const auto& state = vm->state();
  std::cout << "Final tensor pool size: " << state.tensors.size() << "\n";
  std::cout << "Free tensor indices: " << state.free_tensor_indices.size() << "\n";

  // With GC working:
  // We allocate 2000 tensors.
  // Only 1 is live at any time.
  // GC runs every 64 instructions.
  // So we should reuse slots.
  // Max active slots should be small (<= 64 + 1).

  if (state.tensors.size() > 200) {
    std::cerr << "GC failed to reclaim tensors! Size: " << state.tensors.size() << "\n";
    return 1;
  }

  std::cout << "GC Test Passed!\n";
  return 0;
}
