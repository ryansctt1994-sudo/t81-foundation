#include <t81/isa/program.hpp>
#include <t81/vm/vm.hpp>
#include "test_runtime_check.hpp"

using namespace t81;

int main() {
  // Program: R1=10, R2=3, R3=DIV R1/R2, R4=MOD R1/R2
  [[maybe_unused]] tisc::Program p;
  p.insns.push_back({tisc::Opcode::LoadImm, 1, 10, 0});
  p.insns.push_back({tisc::Opcode::LoadImm, 2, 3, 0});
  p.insns.push_back({tisc::Opcode::Div, 3, 1, 2});
  p.insns.push_back({tisc::Opcode::Mod, 4, 1, 2});
  p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

  [[maybe_unused]] auto vm = vm::make_interpreter_vm();
  vm->load_program(p);
  [[maybe_unused]] auto res = vm->run_to_halt();
  T81_TEST_CHECK(res.has_value());
  T81_TEST_CHECK(vm->state().contexts[0].registers[3] == 10 / 3);
  T81_TEST_CHECK(vm->state().contexts[0].registers[4] == 10 % 3);

  // Divide by zero should trap
  [[maybe_unused]] tisc::Program bad;
  bad.insns.push_back({tisc::Opcode::LoadImm, 1, 5, 0});
  bad.insns.push_back({tisc::Opcode::LoadImm, 2, 0, 0});
  bad.insns.push_back({tisc::Opcode::Div, 3, 1, 2});
  [[maybe_unused]] auto vm2 = vm::make_interpreter_vm();
  vm2->load_program(bad);
  [[maybe_unused]] auto step = vm2->step();
  T81_TEST_CHECK(step.has_value());  // first load
  step = vm2->step();
  T81_TEST_CHECK(step.has_value());  // second load
  step = vm2->step();
  T81_TEST_CHECK(!step.has_value());
  T81_TEST_CHECK(step.error() == vm::Trap::DivisionFault);

  return 0;
}
