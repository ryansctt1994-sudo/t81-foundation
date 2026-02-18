#include <algorithm>
#include <memory>
#include <string>
#include <t81/axion/engine.hpp>
#include <t81/tisc/program.hpp>
#include <t81/vm/vm.hpp>
#include "test_runtime_check.hpp"

using namespace t81;

namespace {
class DenyEngine : public t81::axion::Engine {
public:
  t81::axion::Verdict evaluate(const t81::axion::SyscallContext&) override {
    return {t81::axion::VerdictKind::Deny, "blocked"};
  }
};
}  // namespace

int main() {
  {
    [[maybe_unused]] tisc::Program p;
    // r1 = 5
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 5, 0});
    // r2 = r1
    p.insns.push_back({tisc::Opcode::Mov, 2, 1, 0});
    // r1++
    p.insns.push_back({tisc::Opcode::Inc, 1, 0, 0});
    // r2--
    p.insns.push_back({tisc::Opcode::Dec, 2, 0, 0});
    // cmp r1,r2 (should set zero=false, negative=false)
    p.insns.push_back({tisc::Opcode::Cmp, 1, 2, 0});
    // push r1, then r2
    p.insns.push_back({tisc::Opcode::Push, 1, 0, 0});
    p.insns.push_back({tisc::Opcode::Push, 2, 0, 0});
    // pop into r3 (expect r2 value)
    p.insns.push_back({tisc::Opcode::Pop, 3, 0, 0});
    // pop into r4 (expect r1 value)
    p.insns.push_back({tisc::Opcode::Pop, 4, 0, 0});
    // Axion ops
    p.insns.push_back({tisc::Opcode::AxRead, 5, 42, 0});
    p.insns.push_back({tisc::Opcode::AxSet, 7, 1, 0});
    p.insns.push_back({tisc::Opcode::AxVerify, 6, 0, 0});
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r = vm->run_to_halt();
    T81_TEST_CHECK(r.has_value());
    T81_TEST_CHECK(vm->state().registers[1] == 6);
    T81_TEST_CHECK(vm->state().registers[2] == 4);
    T81_TEST_CHECK(vm->state().registers[3] == 4);
    T81_TEST_CHECK(vm->state().registers[4] == 6);
    T81_TEST_CHECK(vm->state().registers[5] == 42);
    T81_TEST_CHECK(vm->state().registers[6] == 0);
    T81_TEST_CHECK(vm->state().axion_log.size() >= 3);
    T81_TEST_CHECK(std::any_of(vm->state().axion_log.begin(), vm->state().axion_log.end(),
                               [](const auto& e) { return e.opcode == tisc::Opcode::AxRead; }));
    T81_TEST_CHECK(std::any_of(vm->state().axion_log.begin(), vm->state().axion_log.end(),
                               [](const auto& e) { return e.opcode == tisc::Opcode::AxSet; }));
    T81_TEST_CHECK(std::any_of(vm->state().axion_log.begin(), vm->state().axion_log.end(),
                               [](const auto& e) { return e.opcode == tisc::Opcode::AxVerify; }));
  }

  // Pop with empty stack must trap.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::Pop, 0, 0, 0});
    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto step = vm->step();
    T81_TEST_CHECK(!step.has_value());
    T81_TEST_CHECK(step.error() == vm::Trap::StackFault);
  }

  // Axion privilege denial via custom engine.
  {
    [[maybe_unused]] tisc::Program p;
    p.insns.push_back({tisc::Opcode::AxRead, 0, 1, 0});
    [[maybe_unused]] auto vm = vm::make_interpreter_vm(std::make_unique<DenyEngine>());
    vm->load_program(p);
    [[maybe_unused]] auto res = vm->step();
    T81_TEST_CHECK(!res.has_value());
    T81_TEST_CHECK(res.error() == vm::Trap::SecurityFault);
  }

  // Split/join should emit deterministic Axion trace events.
  {
    [[maybe_unused]] tisc::Program p;
    p.symbol_pool = {"a,,b", ","};
    p.insns.push_back({tisc::Opcode::LoadImm, 1, 1, 0, tisc::LiteralKind::SymbolHandle});
    p.insns.push_back({tisc::Opcode::LoadImm, 2, 2, 0, tisc::LiteralKind::SymbolHandle});
    p.insns.push_back({tisc::Opcode::StrSplit, 3, 1, 2});
    p.insns.push_back({tisc::Opcode::StrJoin, 4, 3, 2});
    p.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    [[maybe_unused]] auto vm = vm::make_interpreter_vm();
    vm->load_program(p);
    [[maybe_unused]] auto r = vm->run_to_halt();
    T81_TEST_CHECK(r.has_value());
    T81_TEST_CHECK(vm->state().register_tags[3] == vm::ValueTag::StringVectorHandle);
    T81_TEST_CHECK(vm->state().register_tags[4] == vm::ValueTag::SymbolHandle);

    const auto& log = vm->state().axion_log;
    T81_TEST_CHECK(std::any_of(log.begin(), log.end(), [](const auto& e) {
      return e.opcode == tisc::Opcode::StrSplit &&
             e.verdict.reason.find("string split") != std::string::npos;
    }));
    T81_TEST_CHECK(std::any_of(log.begin(), log.end(), [](const auto& e) {
      return e.opcode == tisc::Opcode::StrJoin &&
             e.verdict.reason.find("string join") != std::string::npos;
    }));
  }

  return 0;
}
