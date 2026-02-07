#include <cassert>
#include <t81/vm/vm.hpp>
#include <t81/tisc/program.hpp>

using namespace t81;

int main() {
  // Test Neg opcode.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 0, 42, 0});
    p.insns.push_back({tisc::Opcode::Neg, 1, 0, 0});
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r= vm->run_to_halt();
    assert(r.has_value());
    assert(vm->state().registers[1] == -42);
    assert(vm->state().flags.negative);
    assert(!vm->state().flags.zero);
  }

  // Test JumpIfNegative opcode.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 0, -1, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 2, 0, 0});
    p.insns.push_back({tisc::Opcode::Cmp, 0, 2, 0});
    p.insns.push_back({tisc::Opcode::JumpIfNegative, 5, 0, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0}); // should skip
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r= vm->run_to_halt();
    assert(r.has_value());
    assert(vm->state().registers[1] == 0);
  }

  // Test JumpIfPositive opcode.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 0, 1, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 2, 0, 0});
    p.insns.push_back({tisc::Opcode::Cmp, 0, 2, 0});
    p.insns.push_back({tisc::Opcode::JumpIfPositive, 5, 0, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0}); // should skip
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r= vm->run_to_halt();
    assert(r.has_value());
    assert(vm->state().registers[1] == 0);
  }

  // Test JumpIfNegative fall-through.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 0, 1, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 2, 0, 0});
    p.insns.push_back({tisc::Opcode::Cmp, 0, 2, 0});
    p.insns.push_back({tisc::Opcode::JumpIfNegative, 5, 0, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0}); // should not skip
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r= vm->run_to_halt();
    assert(r.has_value());
    assert(vm->state().registers[1] == 99);
  }

  // Test JumpIfPositive fall-through.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 0, -1, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 2, 0, 0});
    p.insns.push_back({tisc::Opcode::Cmp, 0, 2, 0});
    p.insns.push_back({tisc::Opcode::JumpIfPositive, 5, 0, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0}); // should not skip
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r= vm->run_to_halt();
    assert(r.has_value());
    assert(vm->state().registers[1] == 99);
  }

  // Test chained Neg and JumpIfNegative.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 0, 42, 0});
    p.insns.push_back({tisc::Opcode::Neg, 0, 0, 0});
    p.insns.push_back({tisc::Opcode::JumpIfNegative, 4, 0, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0}); // should skip
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r= vm->run_to_halt();
    assert(r.has_value());
    assert(vm->state().registers[1] == 0);
  }

  return 0;
}
