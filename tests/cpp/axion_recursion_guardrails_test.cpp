#include "t81/isa/program.hpp"
#include "t81/vm/vm.hpp"

#include <cstdlib>
#include <iostream>

static void test_policy_recursion_limit_applies_to_call_chain() {
  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "axion_recursion_guardrails_test failure: " << msg << "\n";
      return false;
    }
    return true;
  };

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

  if (!expect(!result.has_value(), "expected trap from recursion limit")) std::exit(1);
  if (!expect(result.error() == t81::vm::Trap::SecurityFault, "expected SecurityFault trap"))
    std::exit(1);
}

int main() {
  test_policy_recursion_limit_applies_to_call_chain();
  std::cout << "axion recursion guardrails tests passed!\n";
  return 0;
}
