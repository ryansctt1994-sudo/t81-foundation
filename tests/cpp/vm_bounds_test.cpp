#include <t81/isa/program.hpp>
#include <t81/vm/vm.hpp>
#include "test_runtime_check.hpp"

using namespace t81;

int main() {
  [[maybe_unused]] tisc::Program p;
  p.insns.push_back({tisc::Opcode::LoadImm, 0, 1, 0});
  p.insns.push_back({tisc::Opcode::Store, 2000, 0, 0});  // way past heap_limit

  [[maybe_unused]] auto vm = vm::make_interpreter_vm();
  vm->load_program(p);
  auto r = vm->step();  // load imm ok
  T81_TEST_CHECK(r.has_value());
  r = vm->step();
  T81_TEST_CHECK(!r.has_value());
  T81_TEST_CHECK(r.error() == vm::Trap::BoundsFault);

  return 0;
}
