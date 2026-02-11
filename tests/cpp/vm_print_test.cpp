#include "test_runtime_check.hpp"
#include <string>

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

  {
    tisc::Program program;
    program.float_pool = {1.25};
    program.fraction_pool = {make_fraction(22, 7)};
    program.symbol_pool = {"alpha"};
    program.insns.push_back({tisc::Opcode::LoadImm, 1, 42, 0});
    program.insns.push_back({tisc::Opcode::LoadImm, 2, 1, 0, tisc::LiteralKind::FloatHandle});
    program.insns.push_back({tisc::Opcode::LoadImm, 3, 1, 0, tisc::LiteralKind::FractionHandle});
    program.insns.push_back({tisc::Opcode::LoadImm, 4, 1, 0, tisc::LiteralKind::SymbolHandle});
    program.insns.push_back({tisc::Opcode::LoadImm, 5, 1, 0, tisc::LiteralKind::Bool});
    program.insns.push_back({tisc::Opcode::LoadImm, 6, 0, 0, tisc::LiteralKind::Bool});
    program.insns.push_back({tisc::Opcode::Print, 1, 0, 0});
    program.insns.push_back({tisc::Opcode::Print, 2, 0, 0});
    program.insns.push_back({tisc::Opcode::Print, 3, 0, 0});
    program.insns.push_back({tisc::Opcode::Print, 4, 0, 0});
    program.insns.push_back({tisc::Opcode::Print, 5, 0, 0});
    program.insns.push_back({tisc::Opcode::Print, 6, 0, 0});
    program.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    [[maybe_unused]] auto run = vm->run_to_halt();
    T81_TEST_CHECK(run.has_value());

    [[maybe_unused]] const auto& out = vm->state().printed_output;
    T81_TEST_CHECK(out.size() == 6);
    T81_TEST_CHECK(out[0] == "42");
    T81_TEST_CHECK(out[1] == "1.25t81");
    T81_TEST_CHECK(out[2] == "22/7t81");
    T81_TEST_CHECK(out[3] == "alpha");
    T81_TEST_CHECK(out[4] == "true");
    T81_TEST_CHECK(out[5] == "false");
  }

  {
    tisc::Program program;
    program.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0, tisc::LiteralKind::FloatHandle});
    program.insns.push_back({tisc::Opcode::Print, 1, 0, 0});
    auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    [[maybe_unused]] auto run = vm->run_to_halt();
    T81_TEST_CHECK(!run.has_value());
    T81_TEST_CHECK(run.error() == vm::Trap::TypeFault);
  }

  return 0;
}
