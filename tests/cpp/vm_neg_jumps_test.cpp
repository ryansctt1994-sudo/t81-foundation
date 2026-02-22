#include <t81/tisc/program.hpp>
#include <t81/vm/vm.hpp>
#include "test_runtime_check.hpp"

using namespace t81;

int main() {
  // Test Neg opcode.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 42, 0});
    p.insns.push_back({tisc::Opcode::Neg, 2, 1, 0});
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r = vm->run_to_halt();
    T81_TEST_CHECK(r.has_value());
    T81_TEST_CHECK(vm->state().contexts[0].registers[2] == -42);
    T81_TEST_CHECK(vm->state().contexts[0].flags.negative);
    T81_TEST_CHECK(!vm->state().contexts[0].flags.zero);
  }

  // Test JumpIfNegative opcode.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 3, -1, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 2, 0, 0});
    p.insns.push_back({tisc::Opcode::Cmp, 3, 2, 0});
    p.insns.push_back({tisc::Opcode::JumpIfNegative, 5, 0, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0});  // should skip
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r = vm->run_to_halt();
    T81_TEST_CHECK(r.has_value());
    T81_TEST_CHECK(vm->state().contexts[0].registers[1] == 0);
  }

  // Test JumpIfPositive opcode.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 3, 1, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 2, 0, 0});
    p.insns.push_back({tisc::Opcode::Cmp, 3, 2, 0});
    p.insns.push_back({tisc::Opcode::JumpIfPositive, 5, 0, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0});  // should skip
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r = vm->run_to_halt();
    T81_TEST_CHECK(r.has_value());
    T81_TEST_CHECK(vm->state().contexts[0].registers[1] == 0);
  }

  // Test JumpIfNegative fall-through.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 3, 1, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 2, 0, 0});
    p.insns.push_back({tisc::Opcode::Cmp, 3, 2, 0});
    p.insns.push_back({tisc::Opcode::JumpIfNegative, 5, 0, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0});  // should not skip
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r = vm->run_to_halt();
    T81_TEST_CHECK(r.has_value());
    T81_TEST_CHECK(vm->state().contexts[0].registers[1] == 99);
  }

  // Test JumpIfPositive fall-through.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 3, -1, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 2, 0, 0});
    p.insns.push_back({tisc::Opcode::Cmp, 3, 2, 0});
    p.insns.push_back({tisc::Opcode::JumpIfPositive, 5, 0, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0});  // should not skip
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r = vm->run_to_halt();
    T81_TEST_CHECK(r.has_value());
    T81_TEST_CHECK(vm->state().contexts[0].registers[1] == 99);
  }

  // Test chained Neg and JumpIfNegative.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::LoadImm, 3, 42, 0});
    p.insns.push_back({tisc::Opcode::Neg, 3, 3, 0});
    p.insns.push_back({tisc::Opcode::JumpIfNegative, 4, 0, 0});
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0});  // should skip
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r = vm->run_to_halt();
    T81_TEST_CHECK(r.has_value());
    T81_TEST_CHECK(vm->state().contexts[0].registers[1] == 0);
  }

  return 0;
}
