#include <cassert>
#include <iostream>
#include <vector>
#include "t81/frontend/ir_generator.hpp"
#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include "t81/isa/ir.hpp"

using namespace t81::frontend;
using namespace t81::tisc::ir;

#define EXPECT(cond, msg)                                   \
  if (!(cond)) {                                            \
    std::cerr << "FAIL: " << msg << " (" << #cond << ")\n"; \
    std::exit(1);                                           \
  }

void test_logical_and_success() {
  std::string source = "let x = true && false;";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();

  IRGenerator generator;
  auto program = generator.generate(stmts);
  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "AND should produce instructions");

  // Verify structure: LOADI 0 -> JZ -> JZ -> LOADI 1 -> LABEL
  bool has_loadi_0 = false;
  int jz_count = 0;
  bool has_loadi_1 = false;
  bool has_label = false;

  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::LOADI) {
      if (auto* imm = std::get_if<t81::tisc::ir::Immediate>(&inst.operands[1])) {
        if (imm->value == 0) has_loadi_0 = true;
        if (imm->value == 1) has_loadi_1 = true;
      }
    }
    if (inst.opcode == Opcode::JZ) {
      jz_count++;
    }
    if (inst.opcode == Opcode::LABEL) {
      has_label = true;
    }
  }

  EXPECT(has_loadi_0, "AND should init with 0 (false)");
  EXPECT(jz_count >= 2, "AND should have at least 2 JZ instructions");
  EXPECT(has_loadi_1, "AND should set 1 (true) on success path");
  EXPECT(has_label, "AND should emit end label");

  std::cout << "test_logical_and_success passed!\n";
}

void test_logical_or_success() {
  std::string source = "let x = true || false;";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();

  IRGenerator generator;
  auto program = generator.generate(stmts);
  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "OR should produce instructions");

  // Verify structure: LOADI 1 -> JNZ -> JNZ -> LOADI 0 -> LABEL
  bool has_loadi_0 = false;
  bool has_loadi_1 = false;
  int jnz_count = 0;
  bool has_label = false;

  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::LOADI) {
      if (auto* imm = std::get_if<t81::tisc::ir::Immediate>(&inst.operands[1])) {
        if (imm->value == 0) has_loadi_0 = true;
        if (imm->value == 1) has_loadi_1 = true;
      }
    }
    if (inst.opcode == Opcode::JNZ) {
      jnz_count++;
    }
    if (inst.opcode == Opcode::LABEL) {
      has_label = true;
    }
  }

  EXPECT(has_loadi_1, "OR should init with 1 (true)");
  EXPECT(jnz_count >= 2, "OR should have at least 2 JNZ instructions");
  EXPECT(has_loadi_0, "OR should set 0 (false) on fail path");
  EXPECT(has_label, "OR should emit end label");

  std::cout << "test_logical_or_success passed!\n";
}

void test_nested_logical() {
  std::string source = "let x = (true || false) && true;";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();

  IRGenerator generator;
  auto program = generator.generate(stmts);
  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "Nested logical ops should compile");

  int jz_count = 0;
  int jnz_count = 0;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::JZ) jz_count++;
    if (inst.opcode == Opcode::JNZ) jnz_count++;
  }

  EXPECT(jz_count >= 2, "Nested AND should produce JZs");
  EXPECT(jnz_count >= 2, "Nested OR should produce JNZs");

  std::cout << "test_nested_logical passed!\n";
}

int main() {
  test_logical_and_success();
  test_logical_or_success();
  test_nested_logical();
  std::cout << "All logical lowering tests passed!\n";
  return 0;
}
