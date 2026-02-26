#include <string>
#include "test_runtime_check.hpp"

#include "t81/isa/program.hpp"
#include "t81/vm/state.hpp"
#include "t81/vm/vm.hpp"

int main() {
  using namespace t81;

  {
    // Test AxReport and AxCheck
    tisc::Program program;
    program.symbol_pool = {"ReportMessage", "CheckPassed", "CheckFailed"};

    // AxReport("ReportMessage")
    // r1 = "ReportMessage" (symbol handle)
    // AxReport r1
    program.insns.push_back({tisc::Opcode::LoadImm, 1, 1, 0, tisc::LiteralKind::SymbolHandle});
    program.insns.push_back({tisc::Opcode::AxReport, 1, 0, 0});

    // AxCheck(true, "CheckPassed")
    // r2 = 1 (true)
    // r3 = "CheckPassed"
    // AxCheck r2, r3
    program.insns.push_back({tisc::Opcode::LoadImm, 2, 1, 0, tisc::LiteralKind::Int});
    program.insns.push_back({tisc::Opcode::LoadImm, 3, 2, 0, tisc::LiteralKind::SymbolHandle});
    program.insns.push_back({tisc::Opcode::AxCheck, 2, 3, 0});

    // AxCheck(false, "CheckFailed")
    // r4 = 0 (false)
    // r5 = "CheckFailed"
    // AxCheck r4, r5
    program.insns.push_back({tisc::Opcode::LoadImm, 4, 0, 0, tisc::LiteralKind::Int});
    program.insns.push_back({tisc::Opcode::LoadImm, 5, 3, 0, tisc::LiteralKind::SymbolHandle});
    program.insns.push_back({tisc::Opcode::AxCheck, 4, 5, 0});

    program.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

    auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    [[maybe_unused]] auto run = vm->run_to_halt();
    T81_TEST_CHECK(!run.has_value());
    T81_TEST_CHECK(run.error() == vm::Trap::SecurityFault);

    const auto& axion_log = vm->state().axion_log;

    // We expect at least 3 events corresponding to our opcodes.
    bool found_report = false;
    bool found_check_pass = false;
    bool found_check_fail = false;

    for (const auto& event : axion_log) {
      if (event.opcode == tisc::Opcode::AxReport) {
        if (event.verdict.reason.find("AxReport: ReportMessage") != std::string::npos) {
          T81_TEST_CHECK(event.verdict.kind == axion::VerdictKind::Allow);
          found_report = true;
        }
      }
      if (event.opcode == tisc::Opcode::AxCheck) {
        if (event.verdict.reason.find("AxCheck: CheckPassed") != std::string::npos) {
          T81_TEST_CHECK(event.verdict.kind == axion::VerdictKind::Allow);
          T81_TEST_CHECK(event.value == 1);
          found_check_pass = true;
        }
        if (event.verdict.reason.find("AxCheck: CheckFailed") != std::string::npos) {
          T81_TEST_CHECK(event.verdict.kind == axion::VerdictKind::Deny);
          T81_TEST_CHECK(event.value == 0);
          found_check_fail = true;
        }
      }
    }

    T81_TEST_CHECK(found_report);
    T81_TEST_CHECK(found_check_pass);
    T81_TEST_CHECK(found_check_fail);
  }

  return 0;
}
