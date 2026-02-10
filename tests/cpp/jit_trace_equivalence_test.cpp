#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace {

using t81::tisc::Insn;
using t81::tisc::Opcode;
using t81::tisc::Program;
using t81::vm::State;

Program make_hot_arith_program() {
  Program p;
  p.insns = {
      // r1=0, r2=0, r3=1, r4=100 (loop count)
      {Opcode::LoadImm, 1, 0, 0},
      {Opcode::LoadImm, 2, 0, 0},
      {Opcode::LoadImm, 3, 1, 0},
      {Opcode::LoadImm, 4, 100, 0},
      // loop at pc=4:
      {Opcode::Add, 1, 1, 3},       // r1 += 1
      {Opcode::Add, 2, 2, 1},       // r2 += r1
      {Opcode::Sub, 4, 4, 3},       // r4 -= 1
      {Opcode::JumpIfNotZero, 4, 4, 0}, // if r4 != 0 jump loop
      {Opcode::Halt, 0, 0, 0},
  };
  return p;
}

struct Snapshot {
  std::vector<std::int64_t> regs;
  std::vector<t81::vm::ValueTag> tags;
  std::size_t pc{0};
  bool halted{false};
  std::vector<std::string> trace_reasons;
};

Snapshot run_program(const Program& program) {
  auto vm = t81::vm::make_interpreter_vm();
  vm->load_program(program);
  [[maybe_unused]] auto result = vm->run_to_halt();
  assert(result.has_value());

  Snapshot snap;
  const State& st = vm->state();
  snap.regs.assign(st.registers.begin(), st.registers.end());
  snap.tags.assign(st.register_tags.begin(), st.register_tags.end());
  snap.pc = st.pc;
  snap.halted = st.halted;
  snap.trace_reasons.reserve(st.axion_log.size());
  for (const auto& entry : st.axion_log) {
    snap.trace_reasons.push_back(entry.verdict.reason);
  }
  return snap;
}

void test_jit_trace_equivalence_and_determinism() {
  const Program program = make_hot_arith_program();

  // Both runs should converge to identical final machine states and traces,
  // regardless of when the internal trace JIT engages.
  const Snapshot a = run_program(program);
  const Snapshot b = run_program(program);

  assert(a.regs == b.regs);
  assert(a.tags == b.tags);
  assert(a.pc == b.pc);
  assert(a.halted == b.halted);
  assert(a.trace_reasons == b.trace_reasons);
  bool saw_enter = false;
  bool saw_exit = false;
  for (const auto& reason : a.trace_reasons) {
    if (reason.find("jit trace enter") != std::string::npos) {
      saw_enter = true;
    }
    if (reason.find("jit trace exit") != std::string::npos) {
      saw_exit = true;
    }
  }
  if (!saw_enter || !saw_exit) {
    std::cerr << "missing jit boundary reasons: enter=" << saw_enter
              << " exit=" << saw_exit << "\n";
    assert(false);
  }

  // Sanity-check the arithmetic result for this loop.
  // r1 = 100, r2 = 1 + ... + 100 = 5050.
  assert(a.regs[1] == 100);
  assert(a.regs[2] == 5050);
}

}  // namespace

int main() {
  test_jit_trace_equivalence_and_determinism();
  std::cout << "jit trace equivalence test passed\n";
  return 0;
}
