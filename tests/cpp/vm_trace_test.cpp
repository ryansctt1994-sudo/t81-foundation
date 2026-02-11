#include <t81/vm/vm.hpp>
#include <t81/tisc/program.hpp>
#include <iostream>

using namespace t81;

int main() {
  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "vm_trace_test failure: " << msg << "\n";
      return false;
    }
    return true;
  };

  [[maybe_unused]] tisc::Program p;
  for (int i = 0; i < 80; ++i) {
    p.insns.push_back({tisc::Opcode::Nop, 0, 0, 0});
  }
  p.insns.push_back({tisc::Opcode::LoadImm, 0, 1, 0});
  p.insns.push_back({tisc::Opcode::Load, 1, 9999, 0}); // invalid -> trap
  p.axion_policy_text = "(policy (tier 2) (max-stack 1024))";

  [[maybe_unused]] auto vm= vm::make_interpreter_vm();
  vm->load_program(p);
  [[maybe_unused]] auto r1= vm->step();
  if (!expect(r1.has_value(), "first step unexpectedly trapped")) return 1;
  std::expected<void, vm::Trap> r2;
  while (true) {
    r2 = vm->step();
    if (!r2.has_value()) break;
  }
  if (!expect(r2.error() == vm::Trap::BoundsFault, "expected BoundsFault trap")) return 1;
  if (!expect(!vm->state().trace.empty(), "trace buffer unexpectedly empty")) return 1;
  [[maybe_unused]] auto last= vm->state().trace.back();
  if (!expect(last.trap.has_value(), "last trace entry missing trap marker")) return 1;
  if (!expect(vm->state().policy.has_value(), "policy metadata missing")) return 1;
  if (!expect(vm->state().policy->tier == 2, "unexpected loaded policy tier")) return 1;
  if (!expect(vm->state().gc_cycles > 0, "GC cycle counter did not advance")) return 1;
  return 0;
}
