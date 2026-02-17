#include "t81/tisc/opcodes.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

#include <vector>
#include "test_runtime_check.hpp"

namespace {
t81::vm::Trap run_expected_trap(const std::vector<t81::tisc::Insn>& insns) {
  [[maybe_unused]] t81::tisc::Program program;
  program.insns = insns;
  [[maybe_unused]] auto vm = t81::vm::make_interpreter_vm();
  vm->load_program(program);
  [[maybe_unused]] auto result = vm->run_to_halt();
  T81_TEST_CHECK(!result.has_value());
  return result.error();
}
}  // namespace

int main() {
  [[maybe_unused]] t81::tisc::Insn load_ten;
  load_ten.opcode = t81::tisc::Opcode::LoadImm;
  load_ten.a = 2;  // R2
  load_ten.b = 10;
  [[maybe_unused]] t81::tisc::Insn load_zero;
  load_zero.opcode = t81::tisc::Opcode::LoadImm;
  load_zero.a = 1;
  load_zero.b = 0;
  [[maybe_unused]] t81::tisc::Insn div;
  div.opcode = t81::tisc::Opcode::Div;
  div.a = 2;
  div.b = 2;
  div.c = 1;
  [[maybe_unused]] t81::tisc::Insn halt;
  halt.opcode = t81::tisc::Opcode::Halt;
  [[maybe_unused]] auto trap_div_zero = run_expected_trap({load_ten, load_zero, div, halt});
  T81_TEST_CHECK(trap_div_zero == t81::vm::Trap::DivisionFault);

  [[maybe_unused]] t81::tisc::Insn load_bad;
  load_bad.opcode = t81::tisc::Opcode::Load;
  load_bad.a = 1;
  load_bad.b = 999999;
  load_bad.c = 0;
  [[maybe_unused]] auto trap_invalid_mem = run_expected_trap({load_bad, halt});
  T81_TEST_CHECK(trap_invalid_mem == t81::vm::Trap::BoundsFault);

  t81::tisc::Insn pop{t81::tisc::Opcode::Pop, {1}};
  [[maybe_unused]] auto trap_bounds = run_expected_trap({pop, halt});
  T81_TEST_CHECK(trap_bounds == t81::vm::Trap::StackFault);

  [[maybe_unused]] t81::tisc::Insn store_bad;
  store_bad.opcode = t81::tisc::Opcode::Store;
  store_bad.a = 999999;
  store_bad.b = 0;
  store_bad.c = 0;
  [[maybe_unused]] auto trap_store_invalid_mem = run_expected_trap({store_bad, halt});
  T81_TEST_CHECK(trap_store_invalid_mem == t81::vm::Trap::BoundsFault);

  [[maybe_unused]] t81::tisc::Insn load_neg;
  load_neg.opcode = t81::tisc::Opcode::Load;
  load_neg.a = 1;
  load_neg.b = -1;
  load_neg.c = 0;
  [[maybe_unused]] auto trap_load_neg = run_expected_trap({load_neg, halt});
  T81_TEST_CHECK(trap_load_neg == t81::vm::Trap::BoundsFault);

  [[maybe_unused]] t81::tisc::Insn store_neg;
  store_neg.opcode = t81::tisc::Opcode::Store;
  store_neg.a = -1;
  store_neg.b = 0;
  store_neg.c = 0;
  [[maybe_unused]] auto trap_store_neg = run_expected_trap({store_neg, halt});
  T81_TEST_CHECK(trap_store_neg == t81::vm::Trap::BoundsFault);

  return 0;
}
