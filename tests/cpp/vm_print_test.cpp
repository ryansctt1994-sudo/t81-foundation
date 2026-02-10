#include <cassert>
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
    program.insns.push_back({tisc::Opcode::Print, 1, 0, 0});
    program.insns.push_back({tisc::Opcode::Print, 2, 0, 0});
    program.insns.push_back({tisc::Opcode::Print, 3, 0, 0});
    program.insns.push_back({tisc::Opcode::Print, 4, 0, 0});
    program.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    [[maybe_unused]] auto run = vm->run_to_halt();
    assert(run.has_value());

    [[maybe_unused]] const auto& out = vm->state().printed_output;
    assert(out.size() == 4);
    assert(out[0] == "42");
    assert(out[1] == "1.25t81");
    assert(out[2] == "22/7t81");
    assert(out[3] == "alpha");
  }

  {
    tisc::Program program;
    program.insns.push_back({tisc::Opcode::LoadImm, 1, 99, 0, tisc::LiteralKind::FloatHandle});
    program.insns.push_back({tisc::Opcode::Print, 1, 0, 0});
    auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    [[maybe_unused]] auto run = vm->run_to_halt();
    assert(!run.has_value());
    assert(run.error() == vm::Trap::TypeFault);
  }

  return 0;
}
