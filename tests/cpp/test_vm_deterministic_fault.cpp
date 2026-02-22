#include "t81/tisc/opcodes.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

#include <vector>
#include "test_runtime_check.hpp"

namespace {
// Helper to create a simple program
t81::tisc::Program create_program(const std::vector<t81::tisc::Insn>& insns) {
  t81::tisc::Program program;
  program.insns = insns;
  return program;
}
}  // namespace

int main() {
  using namespace t81::tisc;
  using namespace t81::vm;

  // Test 1: Fault injection at instruction count 1
  {
    std::vector<Insn> insns = {
        Insn{Opcode::LoadImm, 1, 10},  // inst 0
        Insn{Opcode::LoadImm, 2, 20},  // inst 1
        Insn{Opcode::Add, 3, 1, 2},    // inst 2
        Insn{Opcode::Halt}             // inst 3
    };

    auto vm = make_interpreter_vm();
    vm->load_program(create_program(insns));

    // Inject fault at instruction count 1 (the second LoadImm)
    std::vector<FaultInjection> faults = {
        {1, Trap::DivisionFault}  // Arbitrary trap
    };
    vm->set_fault_injections(faults);

    // Step 0: LoadImm R1, 10
    auto res = vm->step();
    T81_TEST_CHECK(res.has_value());
    T81_TEST_CHECK(vm->state().contexts[0].registers[1] == 10);

    // Step 1: Should trap
    res = vm->step();
    T81_TEST_CHECK(!res.has_value());
    T81_TEST_CHECK(res.error() == Trap::DivisionFault);

    // Ensure it didn't execute
    T81_TEST_CHECK(vm->state().contexts[0].registers[2] == 0);
  }

  // Test 2: Fault injection at instruction count 0
  {
    std::vector<Insn> insns = {Insn{Opcode::LoadImm, 1, 10}, Insn{Opcode::Halt}};

    auto vm = make_interpreter_vm();
    vm->load_program(create_program(insns));

    std::vector<FaultInjection> faults = {{0, Trap::SecurityFault}};
    vm->set_fault_injections(faults);

    auto res = vm->step();
    T81_TEST_CHECK(!res.has_value());
    T81_TEST_CHECK(res.error() == Trap::SecurityFault);
    T81_TEST_CHECK(vm->state().contexts[0].registers[1] == 0);
  }

  // Test 3: Multiple faults
  {
    std::vector<Insn> insns = {Insn{Opcode::Nop},  // 0
                               Insn{Opcode::Nop},  // 1
                               Insn{Opcode::Nop},  // 2
                               Insn{Opcode::Halt}};

    auto vm = make_interpreter_vm();
    vm->load_program(create_program(insns));

    std::vector<FaultInjection> faults = {{0, Trap::TypeFault}, {2, Trap::StackFault}};
    vm->set_fault_injections(faults);

    // Trap at 0
    auto res = vm->step();
    T81_TEST_CHECK(!res.has_value());
    T81_TEST_CHECK(res.error() == Trap::TypeFault);

    // Since we know the VM implementation doesn't advance PC/instruction count on fault injection
    // return, we can't easily test the second fault without resetting or manual intervention which
    // is internal. So we just verify the first fault works.
  }

  return 0;
}
