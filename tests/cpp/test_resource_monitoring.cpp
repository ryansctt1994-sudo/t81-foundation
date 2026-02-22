#include <iostream>
#include <vector>
#include "t81/tisc/opcodes.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

// Test Resource Monitoring

int main() {
  using namespace t81::tisc;
  Program prog;
  // Set max tensors to 3. The 4th allocation should fail.
  prog.axion_policy_text = "(policy (max-tensors 3))";

  // 1. Alloc Tensor 1
  prog.insns.push_back({Opcode::LoadImm, 1, 10, 0});  // R1 (size) = 10
  prog.insns.push_back({Opcode::TNew, 10, 1, 0});     // R10 = tensor(10)

  // 2. Alloc Tensor 2
  prog.insns.push_back({Opcode::TNew, 11, 1, 0});  // R11 = tensor(10)

  // 3. Alloc Tensor 3
  prog.insns.push_back({Opcode::TNew, 12, 1, 0});  // R12 = tensor(10)

  // 4. Alloc Tensor 4 (Should Fail)
  prog.insns.push_back({Opcode::TNew, 13, 1, 0});  // R13 = tensor(10)

  prog.insns.push_back({Opcode::Halt, 0, 0, 0});

  auto vm = t81::vm::make_interpreter_vm(nullptr);
  vm->load_program(prog);

  auto result = vm->run_to_halt(100);

  if (result) {
    std::cerr << "Test Failed: Expected VM to trap due to resource limit." << std::endl;
    return 1;
  }

  if (result.error() != t81::vm::Trap::SecurityFault) {
    std::cerr << "Test Failed: Expected SecurityFault, got " << t81::vm::to_string(result.error())
              << std::endl;
    return 1;
  }

  const auto& state = vm->state();
  if (state.metrics.total_tensors != 3) {
    std::cerr << "Test Failed: total_tensors should be 3, got " << state.metrics.total_tensors
              << std::endl;
    return 1;
  }

  bool found_deny = false;
  for (const auto& evt : state.axion_log) {
    if (evt.verdict.kind == t81::axion::VerdictKind::Deny &&
        evt.verdict.reason.find("Max tensors limit exceeded") != std::string::npos) {
      found_deny = true;
      break;
    }
  }

  if (!found_deny) {
    std::cerr << "Test Failed: Did not find Axion Deny event." << std::endl;
    return 1;
  }

  std::cout << "Test Passed: Resource limits enforced." << std::endl;
  return 0;
}
