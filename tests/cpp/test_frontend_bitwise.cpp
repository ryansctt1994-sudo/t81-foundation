#include "t81/frontend/ir_generator.hpp"
#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include "t81/tisc/ir.hpp"

#include <cassert>
#include <iostream>
#include <vector>

using namespace t81::frontend;
using namespace t81::tisc::ir;

#define EXPECT(cond, msg)                                   \
  if (!(cond)) {                                            \
    std::cerr << "FAIL: " << msg << " (" << #cond << ")\n"; \
    std::exit(1);                                           \
  }

void test_bitwise_and() {
  std::string source = "let x = 1 & 2;";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();
  EXPECT(!parser.had_error(), "Parser failed for bitwise AND");

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  EXPECT(!analyzer.had_error(), "Semantic analysis failed for bitwise AND");

  IRGenerator generator;
  generator.attach_semantic_analyzer(&analyzer);
  auto program = generator.generate(stmts);

  bool found = false;
  for (const auto& inst : program.instructions()) {
    if (inst.opcode == Opcode::BITAND) found = true;
  }
  EXPECT(found, "Expected BITAND opcode");
  std::cout << "test_bitwise_and passed\n";
}

void test_bitwise_or() {
  std::string source = "let x = 1 | 2;";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();
  EXPECT(!parser.had_error(), "Parser failed for bitwise OR");

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  EXPECT(!analyzer.had_error(), "Semantic analysis failed for bitwise OR");

  IRGenerator generator;
  generator.attach_semantic_analyzer(&analyzer);
  auto program = generator.generate(stmts);

  bool found = false;
  for (const auto& inst : program.instructions()) {
    if (inst.opcode == Opcode::BITOR) found = true;
  }
  EXPECT(found, "Expected BITOR opcode");
  std::cout << "test_bitwise_or passed\n";
}

void test_bitwise_xor() {
  std::string source = "let x = 1 ^ 2;";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();
  EXPECT(!parser.had_error(), "Parser failed for bitwise XOR");

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  EXPECT(!analyzer.had_error(), "Semantic analysis failed for bitwise XOR");

  IRGenerator generator;
  generator.attach_semantic_analyzer(&analyzer);
  auto program = generator.generate(stmts);

  bool found = false;
  for (const auto& inst : program.instructions()) {
    if (inst.opcode == Opcode::BITXOR) found = true;
  }
  EXPECT(found, "Expected BITXOR opcode");
  std::cout << "test_bitwise_xor passed\n";
}

void test_bitwise_not() {
  std::string source = "let x = ~1;";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();
  EXPECT(!parser.had_error(), "Parser failed for bitwise NOT");

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  EXPECT(!analyzer.had_error(), "Semantic analysis failed for bitwise NOT");

  IRGenerator generator;
  generator.attach_semantic_analyzer(&analyzer);
  auto program = generator.generate(stmts);

  bool found = false;
  for (const auto& inst : program.instructions()) {
    if (inst.opcode == Opcode::BITNOT) found = true;
  }
  EXPECT(found, "Expected BITNOT opcode");
  std::cout << "test_bitwise_not passed\n";
}

void test_shifts() {
  std::string source = R"(
    let a = 1 << 2;
    let b = 2 >> 1;
    let c = 4 >>> 1;
  )";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();
  EXPECT(!parser.had_error(), "Parser failed for shifts");

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  EXPECT(!analyzer.had_error(), "Semantic analysis failed for shifts");

  IRGenerator generator;
  generator.attach_semantic_analyzer(&analyzer);
  auto program = generator.generate(stmts);

  bool has_shl = false, has_shr = false, has_ushr = false;
  for (const auto& inst : program.instructions()) {
    if (inst.opcode == Opcode::BITSHL) has_shl = true;
    if (inst.opcode == Opcode::BITSHR) has_shr = true;
    if (inst.opcode == Opcode::BITUSHR) has_ushr = true;
  }
  EXPECT(has_shl, "Expected BITSHL opcode");
  EXPECT(has_shr, "Expected BITSHR opcode");
  EXPECT(has_ushr, "Expected BITUSHR opcode");
  std::cout << "test_shifts passed\n";
}

void test_precedence() {
  // 1 << 1 + 1 -> 1 << 2 = 4
  std::string source2 = "let x = 1 << 1 + 1;";

  Lexer lexer(source2);
  Parser parser(lexer);
  auto stmts = parser.parse();
  EXPECT(!parser.had_error(), "Parser failed for precedence");

  IRGenerator generator;
  auto program = generator.generate(stmts);

  std::cout << "test_precedence passed (syntax check only)\n";
}

void test_type_error() {
  std::string source = "let x = 1.0 & 2;";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();
  // Parser passes.

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  EXPECT(analyzer.had_error(), "Semantic analysis should fail for float bitwise");

  std::cout << "test_type_error passed\n";
}

int main() {
  test_bitwise_and();
  test_bitwise_or();
  test_bitwise_xor();
  test_bitwise_not();
  test_shifts();
  test_precedence();
  test_type_error();
  return 0;
}
