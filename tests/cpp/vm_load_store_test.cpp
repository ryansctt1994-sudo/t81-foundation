#include <t81/tisc/program.hpp>
#include <t81/vm/vm.hpp>
#include "test_runtime_check.hpp"

using namespace t81;

int main() {
  // Program: load imm 7 into R1; store to mem[5]; load mem[5] into R2; halt.
  [[maybe_unused]] tisc::Program p;
  p.insns.push_back({tisc::Opcode::LoadImm, 1, 7, 0});
  p.insns.push_back({tisc::Opcode::Store, 5, 1, 0});
  p.insns.push_back({tisc::Opcode::Load, 2, 5, 0});
  p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

  [[maybe_unused]] auto vm = vm::make_interpreter_vm();
  vm->load_program(p);
  [[maybe_unused]] auto res = vm->run_to_halt();
  T81_TEST_CHECK(res.has_value());
  T81_TEST_CHECK(vm->state().memory[5] == 7);
  T81_TEST_CHECK(vm->state().contexts[0].registers[2] == 7);

  // Invalid memory should trap
  [[maybe_unused]] tisc::Program bad;
  bad.insns.push_back({tisc::Opcode::Load, 0, 9999, 0});
  bad.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});
  vm->load_program(bad);
  [[maybe_unused]] auto step = vm->step();
  T81_TEST_CHECK(!step.has_value());
  T81_TEST_CHECK(step.error() == vm::Trap::BoundsFault);

  return 0;
}
