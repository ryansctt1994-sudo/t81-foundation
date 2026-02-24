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

void run_expect_success(const std::vector<t81::tisc::Insn>& insns) {
  t81::vm::Trap t = run_until_trap(insns);
  assert(t == t81::vm::Trap::None);
  (void)t;
}
}  // namespace

int main() {
  // Test TNeuralFwd (Forward Pass Identity)
  {
    std::cout << "Testing TNeuralFwd..." << std::endl;
    std::vector<t81::tisc::Insn> prog;

    // R1 = 42
    t81::tisc::Insn load_imm;
    load_imm.opcode = t81::tisc::Opcode::LoadImm;
    load_imm.a = 1;
    load_imm.b = 42;
    prog.push_back(load_imm);

    // R2 = TNeuralFwd(R1) -> should be 42 (identity)
    t81::tisc::Insn fwd;
    fwd.opcode = t81::tisc::Opcode::TNeuralFwd;
    fwd.a = 2;  // Dest
    fwd.b = 1;  // Src
    prog.push_back(fwd);

    // R3 = R2 - R1
    t81::tisc::Insn sub;
    sub.opcode = t81::tisc::Opcode::Sub;
    sub.a = 3;
    sub.b = 2;
    sub.c = 1;
    prog.push_back(sub);

    // If R3 == 0, Jump to Halt (index 5)
    t81::tisc::Insn jmp_success;
    jmp_success.opcode = t81::tisc::Opcode::JumpIfZero;
    jmp_success.a = 5;
    jmp_success.b = 3;
    prog.push_back(jmp_success);

    // Fail: Trap
    t81::tisc::Insn trap;
    trap.opcode = t81::tisc::Opcode::Trap;
    prog.push_back(trap);

    // Success label (index 5)
    t81::tisc::Insn halt;
    halt.opcode = t81::tisc::Opcode::Halt;
    prog.push_back(halt);

    run_expect_success(prog);
  }

  // Test TNeuralBwd (Backward Pass Stub)
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

    t81::tisc::Insn halt;
    halt.opcode = t81::tisc::Opcode::Halt;
    prog.push_back(halt);

    run_expect_success(prog);
  }

  std::cout << "test_vm_neural_opcodes PASSED\n";
  return 0;
}
