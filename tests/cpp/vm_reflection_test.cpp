#include "t81/tisc/opcodes.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/state.hpp"
#include "t81/vm/vm.hpp"

#include <iostream>
#include <vector>
#include "test_runtime_check.hpp"

namespace {

std::unique_ptr<t81::vm::IVirtualMachine> run_program(const std::vector<t81::tisc::Insn>& insns) {
  t81::tisc::Program program;
  program.insns = insns;
  auto vm = t81::vm::make_interpreter_vm();
  vm->load_program(program);
  auto result = vm->run_to_halt();
  if (!result.has_value()) {
    std::cerr << "Execution failed with trap: " << t81::vm::to_string(result.error()) << "\n";
  }
  T81_TEST_CHECK(result.has_value());
  return vm;
}

}  // namespace

int main() {
  using namespace t81::tisc;
  using namespace t81::vm;

  std::cout << "Testing MetaRead/MetaWrite for Registers...\n";
  {
    Insn i1{Opcode::LoadImm, 1, 42};
    Insn i2{Opcode::LoadImm, 2, 1};
    Insn i3{Opcode::MetaRead, 3, static_cast<int64_t>(MemorySegmentKind::Registers), 2};
    Insn i4{Opcode::Halt};

    auto vm = run_program({i1, i2, i3, i4});
    T81_TEST_CHECK(vm->state().registers[3] == 42);
    std::cout << "  MetaRead Registers: PASS\n";

    Insn i5{Opcode::LoadImm, 4, 81};
    Insn i6{Opcode::LoadImm, 5, 6};
    Insn i7{Opcode::MetaWrite, 4, static_cast<int64_t>(MemorySegmentKind::Registers), 5};
    Insn i8{Opcode::Halt};

    vm = run_program({i5, i6, i7, i8});
    T81_TEST_CHECK(vm->state().registers[6] == 81);
    std::cout << "  MetaWrite Registers: PASS\n";
  }

  std::cout << "Testing MetaRead/MetaWrite for Code (Self-Patching)...\n";
  {
    Insn load_r1{Opcode::LoadImm, 1, 10};
    Insn add_r0{Opcode::Add, 0, 1, 1};
    Insn load_addr{Opcode::LoadImm, 2, 6};  // 1*4 + 2
    Insn load_val{Opcode::LoadImm, 3, 100};
    Insn patch{Opcode::MetaWrite, 3, static_cast<int64_t>(MemorySegmentKind::Code), 2};
    Insn jump_back{Opcode::Jump, 1};
    Insn halt{Opcode::Halt};

    t81::tisc::Program program;
    program.insns = {load_r1, add_r0, load_addr, load_val, patch, jump_back, halt};
    auto vm = t81::vm::make_interpreter_vm();
    vm->load_program(program);
    vm->set_register(100, 1000, ValueTag::Int);

    vm->step();  // PC 0
    vm->step();  // PC 1
    T81_TEST_CHECK(vm->state().registers[1] == 20);
    vm->step();  // PC 2
    vm->step();  // PC 3
    vm->step();  // PC 4 (patch)
    vm->step();  // PC 5 (jump 1)
    T81_TEST_CHECK(vm->state().pc == 1);
    vm->step();  // PC 1 (patched)
    T81_TEST_CHECK(vm->state().registers[1] == 1010);

    std::cout << "  Self-Patching (CODE segment): PASS\n";
  }

  std::cout << "Testing MetaReflect/MetaRefine...\n";
  {
    Insn reflect{Opcode::MetaReflect, 1, 0};
    Insn refine{Opcode::MetaRefine, 2, 1, 1};
    Insn halt{Opcode::Halt};

    auto vm = run_program({reflect, refine, halt});
    T81_TEST_CHECK(vm->state().registers[1] != 0);
    T81_TEST_CHECK(vm->state().registers[2] == 0);
    std::cout << "  MetaReflect/MetaRefine: PASS\n";
  }

  std::cout << "All reflection tests passed!\n";
  return 0;
}
