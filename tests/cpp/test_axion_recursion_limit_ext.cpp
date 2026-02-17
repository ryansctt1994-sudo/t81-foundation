#include <cassert>
#include <iostream>
#include "t81/axion/policy.hpp"
#include "t81/axion/policy_engine.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

void test_recursion_limit() {
  [[maybe_unused]] std::string policy_text = "(policy (max-recursion 5))";
  [[maybe_unused]] auto policy_opt = t81::axion::parse_policy(policy_text);
  assert(policy_opt.has_value());
  assert(policy_opt->max_recursion == 5);

  [[maybe_unused]] auto engine = t81::axion::make_policy_engine(policy_opt.value());
  [[maybe_unused]] auto vm = t81::vm::make_interpreter_vm(std::move(engine));

  [[maybe_unused]] t81::tisc::Program program;
  program.axion_policy_text = policy_text;
  // Simple recursive call simulation using StackAlloc to simulate frames
  program.insns = {
      {t81::tisc::Opcode::StackAlloc, 0, 10, 0},
      {t81::tisc::Opcode::StackAlloc, 0, 10, 0},
      {t81::tisc::Opcode::StackAlloc, 0, 10, 0},
      {t81::tisc::Opcode::StackAlloc, 0, 10, 0},
      {t81::tisc::Opcode::StackAlloc, 0, 10, 0},
      {t81::tisc::Opcode::StackAlloc, 0, 10, 0},  // 6th frame should be denied
      {t81::tisc::Opcode::Halt, 0, 0, 0},
  };

  vm->load_program(program);
  [[maybe_unused]] auto result = vm->run_to_halt();

  assert(!result.has_value());
  assert(result.error() == t81::vm::Trap::SecurityFault);

  std::cout << "test_recursion_limit passed!" << std::endl;
}

int main() {
  test_recursion_limit();
  return 0;
}
