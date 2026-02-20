#include "t81/tisc/opcodes.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"
#include "t81/vm/traps.hpp"

#include <vector>
#include "test_runtime_check.hpp"

namespace {
t81::vm::Trap run_until_trap(const std::vector<t81::tisc::Insn>& insns) {
  t81::tisc::Program program;
  program.insns = insns;
  auto vm = t81::vm::make_interpreter_vm();
  vm->load_program(program);
  auto result = vm->run_to_halt();
  if (!result.has_value()) {
    return result.error();
  }
  return t81::vm::Trap::None;
}

void run_expect_success(const std::vector<t81::tisc::Insn>& insns) {
  t81::vm::Trap t = run_until_trap(insns);
  T81_TEST_CHECK(t == t81::vm::Trap::None);
}
}  // namespace

int main() {
  // Test TNorm
  {
    std::vector<t81::tisc::Insn> prog;
    // R1 = 10
    t81::tisc::Insn load_pos;
    load_pos.opcode = t81::tisc::Opcode::LoadImm;
    load_pos.a = 1;
    load_pos.b = 10;
    prog.push_back(load_pos);

    // R2 = TNorm(R1) -> should be 1
    t81::tisc::Insn tnorm_pos;
    tnorm_pos.opcode = t81::tisc::Opcode::TNorm;
    tnorm_pos.a = 2;
    tnorm_pos.b = 1;
    prog.push_back(tnorm_pos);

    // Assert R2 != 0 (it is 1)
    t81::tisc::Insn assert_pos;
    assert_pos.opcode = t81::tisc::Opcode::Assert;
    assert_pos.a = 2;
    prog.push_back(assert_pos);

    t81::tisc::Insn halt;
    halt.opcode = t81::tisc::Opcode::Halt;
    prog.push_back(halt);

    run_expect_success(prog);
  }

  // Test Assert failure
  {
    std::vector<t81::tisc::Insn> prog;
    // R1 = 0
    t81::tisc::Insn load_zero;
    load_zero.opcode = t81::tisc::Opcode::LoadImm;
    load_zero.a = 1;
    load_zero.b = 0;
    prog.push_back(load_zero);

    // Assert R1 -> should fail
    t81::tisc::Insn assert_fail;
    assert_fail.opcode = t81::tisc::Opcode::Assert;
    assert_fail.a = 1;
    prog.push_back(assert_fail);

    t81::tisc::Insn halt;
    halt.opcode = t81::tisc::Opcode::Halt;
    prog.push_back(halt);

    t81::vm::Trap t = run_until_trap(prog);
    T81_TEST_CHECK(t == t81::vm::Trap::AssertionFailed);
  }

  // Test AxHalt
  {
    std::vector<t81::tisc::Insn> prog;
    t81::tisc::Insn axhalt;
    axhalt.opcode = t81::tisc::Opcode::AxHalt;
    prog.push_back(axhalt);

    // Should run without crashing, but halt immediately.
    // run_until_trap returns None if it halted normally.
    // AxHalt sets state_.halted = true;
    run_expect_success(prog);
  }

  // Test Cognitive Stub (Recurse)
  {
    std::vector<t81::tisc::Insn> prog;
    t81::tisc::Insn recurse;
    recurse.opcode = t81::tisc::Opcode::Recurse;
    prog.push_back(recurse);

    t81::tisc::Insn halt;
    halt.opcode = t81::tisc::Opcode::Halt;
    prog.push_back(halt);

    run_expect_success(prog);
  }

  return 0;
}
