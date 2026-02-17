// tests/cpp/frontend_ir_generator_test.cpp
// Robust integration tests for IRGenerator against the current frontend.

#include "t81/frontend/ir_generator.hpp"
#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include "t81/tisc/ir.hpp"
#include "t81/tisc/pretty_printer.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace t81::frontend;
using namespace t81::tisc::ir;

#define EXPECT(cond, msg)                                   \
  if (!(cond)) {                                            \
    std::cerr << "FAIL: " << msg << " (" << #cond << ")\n"; \
    std::exit(1);                                           \
  }

void test_simple_addition() {
  [[maybe_unused]] std::string source = "let x = 1 + 2;";
  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();

  [[maybe_unused]] IRGenerator generator;
  [[maybe_unused]] auto program = generator.generate(stmts);

  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "IRGenerator produced no instructions");

  [[maybe_unused]] bool has_loadi = false;
  [[maybe_unused]] bool has_add = false;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::LOADI) has_loadi = true;
    if (inst.opcode == Opcode::ADD) has_add = true;
  }
  EXPECT(has_loadi, "Expected LOADI");
  EXPECT(has_add, "Expected ADD");

  std::cout << "IRGeneratorTest test_simple_addition passed!" << std::endl;
}

void test_if_statement() {
  [[maybe_unused]] std::string source = "if (1 < 2) { let x = 1; }";
  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();

  [[maybe_unused]] IRGenerator generator;
  [[maybe_unused]] auto program = generator.generate(stmts);

  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "IRGenerator produced no instructions for IfStmt");

  [[maybe_unused]] bool found_cmp = false;
  [[maybe_unused]] bool found_jz = false;
  [[maybe_unused]] bool found_label = false;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::CMP) found_cmp = true;
    if (inst.opcode == Opcode::JZ) found_jz = true;
    if (inst.opcode == Opcode::LABEL) found_label = true;
  }
  EXPECT(found_cmp, "Expected CMP for if condition");
  EXPECT(found_jz, "Expected JZ for if branch");
  EXPECT(found_label, "Expected LABEL for if end");

  std::cout << "IRGeneratorTest test_if_statement passed!" << std::endl;
}

void test_if_else_statement() {
  [[maybe_unused]] std::string source = "if (1 < 2) { let x = 1; } else { let y = 2; }";
  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();

  [[maybe_unused]] IRGenerator generator;
  [[maybe_unused]] auto program = generator.generate(stmts);

  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "IRGenerator produced no instructions for IfElseStmt");

  [[maybe_unused]] bool found_jz = false;
  [[maybe_unused]] bool found_jmp = false;
  [[maybe_unused]] int labels_count = 0;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::JZ) found_jz = true;
    if (inst.opcode == Opcode::JMP) found_jmp = true;
    if (inst.opcode == Opcode::LABEL) labels_count++;
  }
  EXPECT(found_jz, "Expected JZ for if branch");
  EXPECT(found_jmp, "Expected JMP to skip else branch");
  EXPECT(labels_count >= 2, "Expected at least 2 labels for if-else");

  std::cout << "IRGeneratorTest test_if_else_statement passed!" << std::endl;
}

void test_while_loop() {
  [[maybe_unused]] std::string source = "while (1 < 2) { let x = 1; }";
  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();

  [[maybe_unused]] IRGenerator generator;
  [[maybe_unused]] auto program = generator.generate(stmts);

  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "IRGenerator produced no instructions for WhileStmt");

  [[maybe_unused]] bool found_jz = false;
  [[maybe_unused]] bool found_jmp = false;
  [[maybe_unused]] int labels_count = 0;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::JZ) found_jz = true;
    if (inst.opcode == Opcode::JMP) found_jmp = true;
    if (inst.opcode == Opcode::LABEL) labels_count++;
  }
  EXPECT(found_jz, "Expected JZ for while condition");
  EXPECT(found_jmp, "Expected JMP back to condition");
  EXPECT(labels_count >= 2, "Expected at least 2 labels for while");

  std::cout << "IRGeneratorTest test_while_loop passed!" << std::endl;
}

void test_loop_statement() {
  [[maybe_unused]] std::string source = "@bounded(5) loop { let x = 1; }";
  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();

  [[maybe_unused]] IRGenerator generator;
  [[maybe_unused]] auto program = generator.generate(stmts);

  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "IRGenerator produced no instructions for LoopStmt");

  [[maybe_unused]] bool found_jmp = false;
  [[maybe_unused]] int labels_count = 0;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::JMP) found_jmp = true;
    if (inst.opcode == Opcode::LABEL) labels_count++;
  }
  EXPECT(found_jmp, "Expected JMP back to start of loop");
  EXPECT(labels_count >= 2, "Expected at least 2 labels for loop (entry/exit)");

  std::cout << "IRGeneratorTest test_loop_statement passed!" << std::endl;
}

void test_guarded_loop_statement() {
  [[maybe_unused]] std::string source = "var x = 0; @bounded(loop(x < 5)) loop { x = x + 1; }";
  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();

  [[maybe_unused]] IRGenerator generator;
  [[maybe_unused]] auto program = generator.generate(stmts);

  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "IRGenerator produced no instructions for Guarded LoopStmt");

  [[maybe_unused]] bool found_jz = false;
  [[maybe_unused]] bool found_jmp = false;
  [[maybe_unused]] int labels_count = 0;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::JZ) found_jz = true;
    if (inst.opcode == Opcode::JMP) found_jmp = true;
    if (inst.opcode == Opcode::LABEL) labels_count++;
  }
  EXPECT(found_jz, "Expected JZ for loop guard");
  EXPECT(found_jmp, "Expected JMP back to guard");
  EXPECT(labels_count >= 3, "Expected at least 3 labels for guarded loop (guard/entry/exit)");

  std::cout << "IRGeneratorTest test_guarded_loop_statement passed!" << std::endl;
}

void test_assignment() {
  [[maybe_unused]] std::string source = "let x = 1; x = 2;";
  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();

  [[maybe_unused]] IRGenerator generator;
  [[maybe_unused]] auto program = generator.generate(stmts);

  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "Assignment should produce IR");

  [[maybe_unused]] bool has_loadi = false;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::LOADI) has_loadi = true;
  }
  EXPECT(has_loadi, "Expected LOADI");

  std::cout << "IRGeneratorTest test_assignment passed!" << std::endl;
}

void test_match_option() {
  std::string source = R"(
        fn main() -> i32 {
            let maybe: Option[i32] = Some(1);
            let v: i32 = match (maybe) {
                Some(x) => x + 1;
                None => 0;
            };
            return v;
        }
    )";
  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();

  [[maybe_unused]] IRGenerator generator;
  [[maybe_unused]] auto program = generator.generate(stmts);
  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "IRGenerator produced no instructions for match");

  [[maybe_unused]] bool has_option_is_some = false;
  [[maybe_unused]] bool has_option_unwrap = false;
  [[maybe_unused]] bool has_branch = false;
  [[maybe_unused]] bool has_jump = false;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::OPTION_IS_SOME) has_option_is_some = true;
    if (inst.opcode == Opcode::OPTION_UNWRAP) has_option_unwrap = true;
    if (inst.opcode == Opcode::JNZ) has_branch = true;
    if (inst.opcode == Opcode::JMP) has_jump = true;
  }

  EXPECT(has_option_is_some, "Option match should emit OPTION_IS_SOME");
  EXPECT(has_option_unwrap, "Option match should unwrap payload");
  EXPECT(has_branch, "Option match should branch");
  EXPECT(has_jump, "Option match should jump to end");

  std::cout << "IRGeneratorTest test_match_option passed!" << std::endl;
}

void test_match_result() {
  std::string source = R"(
        fn main() -> Result[i32, T81String] {
            let result: Result[i32, T81String] = Ok(1);
            return match (result) {
                Ok(x) => Ok(x + 1);
                Err(e) => Err(e);
            };
        }
    )";
  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();

  [[maybe_unused]] IRGenerator generator;
  [[maybe_unused]] auto program = generator.generate(stmts);
  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "Result match should produce IR");

  [[maybe_unused]] bool has_result_is_ok = false;
  [[maybe_unused]] bool has_result_unwrap_ok = false;
  [[maybe_unused]] bool has_result_unwrap_err = false;
  [[maybe_unused]] bool has_branch = false;
  [[maybe_unused]] bool has_jump = false;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::RESULT_IS_OK) has_result_is_ok = true;
    if (inst.opcode == Opcode::RESULT_UNWRAP_OK) has_result_unwrap_ok = true;
    if (inst.opcode == Opcode::RESULT_UNWRAP_ERR) has_result_unwrap_err = true;
    if (inst.opcode == Opcode::JNZ) has_branch = true;
    if (inst.opcode == Opcode::JMP) has_jump = true;
  }

  EXPECT(has_result_is_ok, "Result match should emit RESULT_IS_OK");
  EXPECT(has_result_unwrap_ok, "Result match should unwrap Ok payload");
  EXPECT(has_result_unwrap_err, "Result match should unwrap Err payload");
  EXPECT(has_branch, "Result match should branch");
  EXPECT(has_jump, "Result match should jump to end");

  std::cout << "IRGeneratorTest test_match_result passed!" << std::endl;
}

void test_print_builtin_lowers_to_print_opcode() {
  std::string source = "print(1);";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();

  IRGenerator generator;
  auto program = generator.generate(stmts);
  const auto& instructions = program.instructions();

  EXPECT(!instructions.empty(), "print should produce IR");

  bool has_print = false;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::PRINT) {
      has_print = true;
      break;
    }
  }
  EXPECT(has_print, "Expected PRINT opcode for print(...)");

  std::cout << "IRGeneratorTest test_print_builtin_lowers_to_print_opcode passed!" << std::endl;
}

void test_extended_numeric_types_lower_to_arithmetic_ir() {
  std::string source = R"(
        fn main() -> T81Uint {
            let q: T81Qutrit = 1;
            let q2: T81Qutrit = q + 1;
            let u: T81Uint = 7;
            let u2: T81Uint = u + 2;
            let f: T81Fixed[8, 4] = 3;
            let f2: T81Fixed[8, 4] = f + 4;
            var c1: T81Complex[18];
            var c2: T81Complex[18];
            c2 = c1 + c1;
            return q2 + u2;
        }
    )";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();
  EXPECT(!parser.had_error(), "parser failed for extended numeric types fixture");

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  EXPECT(!analyzer.had_error(), "semantic analyzer failed for extended numeric types fixture");

  IRGenerator generator;
  generator.attach_semantic_analyzer(&analyzer);
  auto program = generator.generate(stmts);
  const auto& instructions = program.instructions();
  EXPECT(!instructions.empty(), "extended numeric fixture produced no IR");

  bool has_add = false;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::ADD) {
      has_add = true;
      break;
    }
  }
  EXPECT(has_add, "extended numeric fixture should lower arithmetic via ADD");
  std::cout << "IRGeneratorTest test_extended_numeric_types_lower_to_arithmetic_ir passed!"
            << std::endl;
}

void test_constructor_and_conversion_calls_lower() {
  std::string source = R"(
        fn main() -> i32 {
            let q: T81Qutrit = T81Qutrit(1);
            let u: T81Uint = T81Uint(q + 2);
            let f: T81Fixed[8, 4] = T81Fixed[8, 4](u);
            let _c: T81Complex[18] = T81Complex[18](u, q);
            return u + 1;
        }
    )";
  Lexer lexer(source);
  Parser parser(lexer);
  auto stmts = parser.parse();
  EXPECT(!parser.had_error(), "parser failed for constructor/conversion fixture");

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  EXPECT(!analyzer.had_error(), "semantic analyzer failed for constructor/conversion fixture");

  IRGenerator generator;
  generator.attach_semantic_analyzer(&analyzer);
  auto program = generator.generate(stmts);
  const auto& instructions = program.instructions();
  EXPECT(!instructions.empty(), "constructor/conversion fixture produced no IR");

  bool has_add = false;
  bool has_make_complex = false;
  for (const auto& inst : instructions) {
    if (inst.opcode == Opcode::ADD) {
      has_add = true;
    }
    if (inst.opcode == Opcode::MAKE_COMPLEX) {
      has_make_complex = true;
    }
  }
  EXPECT(has_add, "constructor/conversion fixture should include ADD");
  EXPECT(has_make_complex, "constructor/conversion fixture should include MAKE_COMPLEX");
  std::cout << "IRGeneratorTest test_constructor_and_conversion_calls_lower passed!"
            << std::endl;
}

int main() {
  test_simple_addition();
  test_if_statement();
  test_if_else_statement();
  test_while_loop();
  test_loop_statement();
  test_guarded_loop_statement();
  test_assignment();
  test_match_option();
  test_match_result();
  test_print_builtin_lowers_to_print_opcode();
  test_extended_numeric_types_lower_to_arithmetic_ir();
  test_constructor_and_conversion_calls_lower();

  std::cout << "All IRGenerator integration tests completed!" << std::endl;
  return 0;
}
