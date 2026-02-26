#include "t81/isa/opcodes.hpp"
#include "t81/isa/program.hpp"
#include "t81/vm/vm.hpp"

#include <iostream>
#include <string_view>

#include "test_runtime_check.hpp"

namespace {

void run_fail_closed_case(t81::tisc::Opcode opcode, std::string_view opname) {
  t81::tisc::Program program;

  t81::tisc::Insn insn{};
  insn.opcode = opcode;
  program.insns.push_back(insn);

  t81::tisc::Insn halt{};
  halt.opcode = t81::tisc::Opcode::Halt;
  program.insns.push_back(halt);

  auto vm = t81::vm::make_interpreter_vm();
  vm->load_program(program);

  auto result = vm->run_to_halt();
  T81_TEST_CHECK(!result.has_value());
  T81_TEST_CHECK(result.error() == t81::vm::Trap::SecurityFault);

  bool saw_block_reason = false;
  for (const auto& event : vm->state().axion_log) {
    if (event.opcode == opcode &&
        event.verdict.reason.find("Blocked: unimplemented privileged Axion opcode") !=
            std::string::npos) {
      saw_block_reason = true;
      break;
    }
  }
  if (!saw_block_reason) {
    std::cerr << "Missing deny-log reason for " << opname << "\n";
  }
  T81_TEST_CHECK(saw_block_reason);
}

}  // namespace

int main() {
  run_fail_closed_case(t81::tisc::Opcode::AxSign, "AXSIGN");
  run_fail_closed_case(t81::tisc::Opcode::AxLineage, "AXLINEAGE");
  run_fail_closed_case(t81::tisc::Opcode::AxCanon, "AXCANON");
  return 0;
}
