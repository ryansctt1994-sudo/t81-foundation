#include "t81/isa/opcodes.hpp"
#include "t81/isa/program.hpp"
#include "t81/vm/traps.hpp"
#include "t81/vm/vm.hpp"

#include <cassert>
#include <iostream>
#include <vector>

namespace {
t81::vm::Trap run_until_trap(const std::vector<t81::tisc::Insn>& insns) {
  t81::tisc::Program program;
  program.insns = insns;
  auto vm = t81::vm::make_interpreter_vm();
  vm->load_program(program);
  auto result = vm->run_to_halt();
  if (!result.has_value()) {
    return result.error();
  }
  return t81::vm::Trap::None;
}

void run_expect_trap(const std::vector<t81::tisc::Insn>& insns, t81::vm::Trap expected) {
  t81::vm::Trap t = run_until_trap(insns);
  assert(t == expected);
  (void)t;
  (void)expected;
}
}  // namespace

int main() {
  // Test TNeuralFwd (fail-closed while unimplemented)
  {
    std::cout << "Testing TNeuralFwd..." << std::endl;
    std::vector<t81::tisc::Insn> prog;

    // R1 = 42
    t81::tisc::Insn load_imm;
    load_imm.opcode = t81::tisc::Opcode::LoadImm;
    load_imm.a = 1;
    load_imm.b = 42;
    prog.push_back(load_imm);

    // TNeuralFwd(R1) should fail closed.
    t81::tisc::Insn fwd;
    fwd.opcode = t81::tisc::Opcode::TNeuralFwd;
    fwd.a = 2;  // Dest (validated but not executed)
    fwd.b = 1;  // Src
    prog.push_back(fwd);
    run_expect_trap(prog, t81::vm::Trap::SecurityFault);
  }

  // Test TNeuralBwd (fail-closed while unimplemented)
  {
    std::cout << "Testing TNeuralBwd..." << std::endl;
    std::vector<t81::tisc::Insn> prog;

    // R1 = 1 (Dummy model handle)
    t81::tisc::Insn load_imm;
    load_imm.opcode = t81::tisc::Opcode::LoadImm;
    load_imm.a = 1;
    load_imm.b = 1;
    prog.push_back(load_imm);

    // TNeuralBwd(R1)
    t81::tisc::Insn bwd;
    bwd.opcode = t81::tisc::Opcode::TNeuralBwd;
    bwd.a = 1;
    prog.push_back(bwd);

    run_expect_trap(prog, t81::vm::Trap::SecurityFault);
  }

  std::cout << "test_vm_neural_opcodes PASSED\n";
  return 0;
}
