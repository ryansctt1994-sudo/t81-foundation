#include <iostream>
#include <memory>
#include <vector>
#include "t81/isa/opcodes.hpp"
#include "t81/isa/program.hpp"
#include "t81/vm/vm.hpp"

int main() {
  using namespace t81::tisc;
  Program prog;

  // We want to overwrite instruction at index 3.
  // 0: LoadImm R1, Opcode::Halt (which is 1)
  // 1: LoadImm R2, 3 (addr)
  // 2: MetaWrite Code (1), R1, R2
  // 3: Nop (Opcode 0) -> will be overwritten to Halt
  // 4: LoadImm R3, 1
  // 5: Halt

  prog.insns.push_back({Opcode::LoadImm, 1, 1, 0, LiteralKind::Int});
  prog.insns.push_back({Opcode::LoadImm, 2, 3, 0, LiteralKind::Int});
  prog.insns.push_back({Opcode::MetaWrite, 1, 1, 2});
  prog.insns.push_back({Opcode::Nop, 0, 0, 0});
  prog.insns.push_back({Opcode::LoadImm, 3, 1, 0, LiteralKind::Int});
  prog.insns.push_back({Opcode::Halt, 0, 0, 0});

  auto vm = t81::vm::make_interpreter_vm(nullptr);
  vm->load_program(prog);

  std::cout << "Running program to test code overwrite protection..." << std::endl;
  auto result = vm->run_to_halt(100);

  if (!result) {
    if (result.error() == t81::vm::Trap::SecurityFault) {
      std::cout << "Protection SUCCESS: Trapped with SecurityFault as expected." << std::endl;
      return 0;
    } else {
      std::cerr << "Protection FAILED: Trapped but with wrong error: "
                << t81::vm::to_string(result.error()) << std::endl;
      return 1;
    }
  }

  // If we reach here, no trap occurred.
  // This means overwrite happened (or didn't happen but execution continued).
  std::cerr << "Protection FAILED: VM did not trap." << std::endl;
  return 1;
}
