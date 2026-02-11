#include <t81/vm/vm.hpp>
#include <t81/tisc/program.hpp>
#include <iostream>

using namespace t81;

int main() {
  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "vm_jump_flags_test failure: " << msg << "\n";
      return false;
    }
    return true;
  };

  // Branch taken when zero; ensures flags and PC update match spec intent.
  {
    [[maybe_unused]] tisc::Program p;
    // pc0: r0 = 0 (sets zero flag)
    p.insns.push_back({tisc::Opcode::LoadImm, 0, 0, 0});
    // pc1: if r0 == 0 jump to pc3
    p.insns.push_back({tisc::Opcode::JumpIfZero, 3, 0, 0});
    // pc2: would set r1 = 1 (should be skipped)
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 1, 0});
    // pc3: halt
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r= vm->run_to_halt();
    if (!expect(r.has_value(), "zero-branch run unexpectedly trapped")) return 1;
    // r1 should remain zero because branch skipped pc2.
    if (!expect(vm->state().registers[1] == 0, "zero-branch did not skip pc2")) return 1;
    // zero flag should reflect last result (from LoadImm r0 = 0).
    if (!expect(vm->state().flags.zero, "zero flag not set")) return 1;
    if (!expect(!vm->state().flags.negative, "negative flag unexpectedly set")) return 1;
  }

  // Invalid jump target should trap and be logged.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::Jump, 5, 0, 0}); // target past program

    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r= vm->step();
    if (!expect(!r.has_value(), "invalid jump did not trap")) return 1;
    if (!expect(r.error() == vm::Trap::DecodeFault, "invalid jump trap type mismatch")) return 1;
    if (!expect(!vm->state().trace.empty(), "trace missing after invalid jump")) return 1;
    if (!expect(vm->state().trace.back().trap.has_value(), "last trace missing trap marker")) return 1;
  }

  // Jump-if-nonzero should branch.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 0, 1, 0});
    p.insns.push_back({tisc::Opcode::JumpIfNotZero, 3, 0, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0}); // should skip
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});
    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r= vm->run_to_halt();
    if (!expect(r.has_value(), "nonzero-branch run unexpectedly trapped")) return 1;
    if (!expect(vm->state().registers[1] == 0, "nonzero-branch did not skip pc2")) return 1;
  }

  // Call/Ret stack handling and Trap instruction.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 0, 4, 0});  // target index
    p.insns.push_back({tisc::Opcode::Call, 0, 0, 0});     // call function at r0
    p.insns.push_back({tisc::Opcode::LoadImm, 2, 7, 0});  // should run after return
    p.insns.push_back({tisc::Opcode::Trap, 1, 0, 0});     // trigger trap
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 42, 0}); // function body
    p.insns.push_back({tisc::Opcode::Ret, 0, 0, 0});

    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r= vm->run_to_halt();
    if (!expect(!r.has_value(), "Trap instruction did not fault")) return 1;
    if (!expect(r.error() == vm::Trap::TrapInstruction, "Trap instruction fault type mismatch")) return 1;
    if (!expect(vm->state().registers[1] == 42, "call body register value mismatch")) return 1;
    if (!expect(vm->state().registers[2] == 7, "post-return register value mismatch")) return 1;
  }

  return 0;
}
