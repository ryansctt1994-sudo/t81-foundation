#include <cmath>
#include "test_runtime_check.hpp"

#include "t81/bigint.hpp"
#include "t81/fraction.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

namespace {
t81::T81Fraction make_fraction(int num, int den) {
  return t81::T81Fraction(t81::T81BigInt::from_i64(num), t81::T81BigInt::from_i64(den));
}
}  // namespace

int main() {
  using namespace t81;

  // Float opcodes produce deterministic handles.
  {
    [[maybe_unused]] tisc::Program program;
    program.float_pool = {1.5, -0.5};
    tisc::Insn f1{tisc::Opcode::LoadImm, 1, 1, 0};
    f1.literal_kind = tisc::LiteralKind::FloatHandle;
    program.insns.push_back(f1);
    tisc::Insn f2{tisc::Opcode::LoadImm, 2, 2, 0};
    f2.literal_kind = tisc::LiteralKind::FloatHandle;
    program.insns.push_back(f2);
    program.insns.push_back({tisc::Opcode::FAdd, 3, 1, 2});
    program.insns.push_back({tisc::Opcode::FSub, 4, 1, 2});
    program.insns.push_back({tisc::Opcode::FMul, 5, 1, 2});
    program.insns.push_back({tisc::Opcode::FDiv, 6, 1, 2});
    program.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    [[maybe_unused]] auto run = vm->run_to_halt();
    T81_TEST_CHECK(run.has_value());
    const auto& floats = vm->state().floats;
    T81_TEST_CHECK(floats.size() == 6);
    auto nearly_equal = [](double lhs, double rhs) { return std::fabs(lhs - rhs) < 1e-12; };
    T81_TEST_CHECK(nearly_equal(floats[2], 1.0));    // FAdd
    T81_TEST_CHECK(nearly_equal(floats[3], 2.0));    // FSub
    T81_TEST_CHECK(nearly_equal(floats[4], -0.75));  // FMul
    T81_TEST_CHECK(nearly_equal(floats[5], -3.0));   // FDiv
  }

  // Float divide-by-zero traps.
  {
    [[maybe_unused]] tisc::Program program;
    program.float_pool = {1.0, 0.0};
    tisc::Insn f1{tisc::Opcode::LoadImm, 1, 1, 0};
    f1.literal_kind = tisc::LiteralKind::FloatHandle;
    program.insns.push_back(f1);
    tisc::Insn f2{tisc::Opcode::LoadImm, 2, 2, 0};
    f2.literal_kind = tisc::LiteralKind::FloatHandle;
    program.insns.push_back(f2);
    program.insns.push_back({tisc::Opcode::FDiv, 3, 1, 2});
    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    [[maybe_unused]] auto run = vm->run_to_halt();
    T81_TEST_CHECK(!run.has_value());
    T81_TEST_CHECK(run.error() == vm::Trap::DivisionFault);
  }

  // Fraction opcodes mirror float behavior.
  {
    [[maybe_unused]] tisc::Program program;
    program.fraction_pool = {make_fraction(1, 2), make_fraction(2, 3)};
    tisc::Insn q1{tisc::Opcode::LoadImm, 1, 1, 0};
    q1.literal_kind = tisc::LiteralKind::FractionHandle;
    program.insns.push_back(q1);
    tisc::Insn q2{tisc::Opcode::LoadImm, 2, 2, 0};
    q2.literal_kind = tisc::LiteralKind::FractionHandle;
    program.insns.push_back(q2);
    program.insns.push_back({tisc::Opcode::FracAdd, 3, 1, 2});
    program.insns.push_back({tisc::Opcode::FracSub, 4, 1, 2});
    program.insns.push_back({tisc::Opcode::FracMul, 5, 1, 2});
    program.insns.push_back({tisc::Opcode::FracDiv, 6, 1, 2});
    program.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    [[maybe_unused]] auto run = vm->run_to_halt();
    T81_TEST_CHECK(run.has_value());
    const auto& fracs = vm->state().fractions;
    T81_TEST_CHECK(fracs.size() == 6);
    T81_TEST_CHECK(fracs[2].num.to_int64() == 7 && fracs[2].den.to_int64() == 6);   // add
    T81_TEST_CHECK(fracs[3].num.to_int64() == -1 && fracs[3].den.to_int64() == 6);  // sub
    T81_TEST_CHECK(fracs[4].num.to_int64() == 1 && fracs[4].den.to_int64() == 3);   // mul
    T81_TEST_CHECK(fracs[5].num.to_int64() == 3 && fracs[5].den.to_int64() == 4);   // div
  }

  // Fraction divide-by-zero traps.
  {
    [[maybe_unused]] tisc::Program program;
    program.fraction_pool = {make_fraction(1, 2), make_fraction(0, 1)};
    tisc::Insn q1{tisc::Opcode::LoadImm, 1, 1, 0};
    q1.literal_kind = tisc::LiteralKind::FractionHandle;
    program.insns.push_back(q1);
    tisc::Insn q2{tisc::Opcode::LoadImm, 2, 2, 0};
    q2.literal_kind = tisc::LiteralKind::FractionHandle;
    program.insns.push_back(q2);
    program.insns.push_back({tisc::Opcode::FracDiv, 3, 1, 2});
    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    [[maybe_unused]] auto run = vm->run_to_halt();
    T81_TEST_CHECK(!run.has_value());
    T81_TEST_CHECK(run.error() == vm::Trap::DivisionFault);
  }

  // Float comparisons influence flags.
  {
    [[maybe_unused]] tisc::Program program;
    program.float_pool = {1.0, 2.0};
    program.insns.push_back({tisc::Opcode::LoadImm, 1, 1, 0, tisc::LiteralKind::FloatHandle});
    program.insns.push_back({tisc::Opcode::LoadImm, 2, 2, 0, tisc::LiteralKind::FloatHandle});
    program.insns.push_back({tisc::Opcode::Cmp, 1, 2, 0});
    program.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});
    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    [[maybe_unused]] auto run = vm->run_to_halt();
    T81_TEST_CHECK(run.has_value());
    T81_TEST_CHECK(vm->state().flags.zero == false);
    T81_TEST_CHECK(vm->state().flags.negative == true);
  }

  // Fraction comparisons influence flags.
  {
    [[maybe_unused]] tisc::Program program;
    program.fraction_pool = {make_fraction(1, 2), make_fraction(3, 4)};
    program.insns.push_back({tisc::Opcode::LoadImm, 1, 1, 0, tisc::LiteralKind::FractionHandle});
    program.insns.push_back({tisc::Opcode::LoadImm, 2, 2, 0, tisc::LiteralKind::FractionHandle});
    program.insns.push_back({tisc::Opcode::Cmp, 2, 1, 0});
    program.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});
    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    [[maybe_unused]] auto run = vm->run_to_halt();
    T81_TEST_CHECK(run.has_value());
    T81_TEST_CHECK(vm->state().flags.zero == false);
    T81_TEST_CHECK(vm->state().flags.negative == false);
  }

  return 0;
}
