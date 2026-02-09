#include "t81/vm/vm.hpp"
#include "t81/tisc/program.hpp"

#include <cassert>
#include <iostream>

static void test_policy_recursion_limit_applies_to_call_chain() {
  t81::tisc::Program program;
  program.axion_policy_text = "(policy (max-recursion 5))";
  program.insns = {
      {t81::tisc::Opcode::LoadImm, 1, 1, 0},  // R1 <- address of call instruction
      {t81::tisc::Opcode::Call, 0, 1, 0},     // recurse forever through R1
      {t81::tisc::Opcode::Halt, 0, 0, 0},
  };

  auto vm = t81::vm::make_interpreter_vm();
  vm->load_program(program);
  auto result = vm->run_to_halt(200);

  assert(!result.has_value());
  assert(result.error() == t81::vm::Trap::SecurityFault);
}

int main() {
  test_policy_recursion_limit_applies_to_call_chain();
  std::cout << "axion recursion guardrails tests passed!\n";
  return 0;
}
