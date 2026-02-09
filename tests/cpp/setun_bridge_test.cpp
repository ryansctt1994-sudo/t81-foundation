#include "t81/setun/bridge.hpp"

#include <cassert>
#include <iostream>

using t81::setun::BridgeError;
using t81::setun::translate_line;
using t81::setun::translate_program;
using t81::tisc::Opcode;

static void test_translate_add_two_address() {
  auto insn = translate_line("ADD R7, R9");
  assert(insn.has_value());
  assert(insn->opcode == Opcode::Add);
  assert(insn->a == 7);
  assert(insn->b == 7);
  assert(insn->c == 9);
}

static void test_translate_program_with_comments() {
  constexpr const char* source = R"(
; Setun compatibility subset
LOADI R1, 10
LOADI R2, 32
ADD R1, R2
JMP 9
HALT # inline comment
)";
  auto program = translate_program(source);
  assert(program.has_value());
  assert(program->insns.size() == 5);
  assert(program->insns[0].opcode == Opcode::LoadImm);
  assert(program->insns[3].opcode == Opcode::Jump);
  assert(program->insns[4].opcode == Opcode::Halt);
}

static void test_unsupported_mnemonic_fails_deterministically() {
  auto insn = translate_line("MUL R1, R2");
  assert(!insn.has_value());
  assert(insn.error() == BridgeError::UnsupportedMnemonic);
}

int main() {
  test_translate_add_two_address();
  test_translate_program_with_comments();
  test_unsupported_mnemonic_fails_deterministically();
  std::cout << "setun bridge tests passed!\n";
  return 0;
}
