#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "t81/isa/opcodes.hpp"
#include "t81/isa/program.hpp"
#include "t81/vm/vm.hpp"

using namespace t81::tisc;
using namespace t81::vm;

void test_reflection_non_interference() {
  std::cout << "Testing that MetaRead/MetaWrite do not perturb Axion logs..." << std::endl;

  // Program that does reflection and then triggers a fault
  std::vector<Insn> insns = {
      /* 0 */ {Opcode::LoadImm, 10, 5},
      /* 1 */ {Opcode::MetaRead, 11, static_cast<int64_t>(MemorySegmentKind::Code), 10},
      /* 2 */ {Opcode::MetaWrite, 11, static_cast<int64_t>(MemorySegmentKind::Code), 10},
      /* 3 */ {Opcode::Load, 0, 9999, 0},  // Trap!
      /* 4 */ {Opcode::Halt},
      /* 5 */ {Opcode::Nop}};

  Program program;
  program.insns = insns;

  auto vm = make_interpreter_vm();
  vm->load_program(program);
  auto result = vm->run_to_halt();
  (void)result;

  assert(!result.has_value());
  assert(result.error() == Trap::BoundsFault);

  bool found_metaread = false;
  bool found_metawrite = false;
  bool found_boundsfault = false;

  for (const auto& event : vm->state().axion_log) {
    if (event.opcode == Opcode::MetaRead) {
      assert(event.verdict.reason.find("MetaRead reflection segment=code addr=5") !=
             std::string::npos);
      found_metaread = true;
    } else if (event.opcode == Opcode::MetaWrite) {
      assert(event.verdict.reason.find("MetaWrite reflection segment=code addr=5") !=
             std::string::npos);
      found_metawrite = true;
    } else if (event.verdict.reason.find("bounds fault segment=unknown addr=9999") !=
               std::string::npos) {
      found_boundsfault = true;
    }
  }

  assert(found_metaread);
  assert(found_metawrite);
  assert(found_boundsfault);
  (void)found_metaread;
  (void)found_metawrite;
  (void)found_boundsfault;

  std::cout << "  Reflection non-interference verified." << std::endl;
}

int main() {
  try {
    test_reflection_non_interference();
    std::cout << "All reflection non-interference tests passed!" << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Test failed with exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
