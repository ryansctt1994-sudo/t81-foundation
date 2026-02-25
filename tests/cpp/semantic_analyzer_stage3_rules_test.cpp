#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"

#include <iostream>
#include <string>

using namespace t81::frontend;

namespace {

bool expect_semantic_success(const std::string& source, const char* label) {
  Lexer lexer(source);
  Parser parser(lexer, label);
  auto stmts = parser.parse();
  if (parser.had_error()) {
    std::cerr << "Unexpected parser error for success fixture: " << label << "\n";
    return false;
  }

  SemanticAnalyzer analyzer(stmts, label);
  analyzer.analyze();
  if (analyzer.had_error()) {
    std::cerr << "Unexpected semantic error for success fixture: " << label << "\n";
    for (const auto& diag : analyzer.diagnostics()) {
      std::cerr << "  - " << diag.message << "\n";
    }
    return false;
  }
  return true;
}

bool expect_semantic_failure(const std::string& source, const char* label) {
  Lexer lexer(source);
  Parser parser(lexer, label);
  auto stmts = parser.parse();
  if (parser.had_error()) {
    return true;
  }

  SemanticAnalyzer analyzer(stmts, label);
  analyzer.analyze();
  if (!analyzer.had_error()) {
    std::cerr << "Expected semantic failure did not occur: " << label << "\n";
    return false;
  }
  return true;
}

}  // namespace

int main() {
  const std::string modulo_integer_success = R"(
    fn main() -> i32 {
      let x: i32 = 9 % 4;
      return x;
    }
  )";
  if (!expect_semantic_success(modulo_integer_success, "modulo_integer_success")) return 1;

  const std::string modulo_fraction_failure = R"(
    fn main() -> i32 {
      let x: T81Fraction = 22/7t81;
      let bad: T81Fraction = x % 2;
      return 0;
    }
  )";
  if (!expect_semantic_failure(modulo_fraction_failure, "modulo_fraction_failure")) return 1;

  const std::string symbol_equality_success = R"(
    fn main() -> i32 {
      let a: Symbol = :foo;
      let b: Symbol = :bar;
      let eq: bool = a == b;
      if (eq) {
        return 1;
      }
      return 0;
    }
  )";
  if (!expect_semantic_success(symbol_equality_success, "symbol_equality_success")) return 1;

  const std::string symbol_relational_failure = R"(
    fn main() -> i32 {
      let a: Symbol = :foo;
      let b: Symbol = :bar;
      if (a < b) {
        return 1;
      }
      return 0;
    }
  )";
  if (!expect_semantic_failure(symbol_relational_failure, "symbol_relational_failure")) return 1;

  const std::string call_widening_success = R"(
    fn takes_float(x: T81Float) -> T81Float {
      return x;
    }

    fn main() -> T81Float {
      let i: i32 = 3;
      return takes_float(i);
    }
  )";
  if (!expect_semantic_success(call_widening_success, "call_widening_success")) return 1;

  const std::string call_narrowing_failure = R"(
    fn takes_int(x: i32) -> i32 {
      return x;
    }

    fn main() -> i32 {
      return takes_int(1.25);
    }
  )";
  if (!expect_semantic_failure(call_narrowing_failure, "call_narrowing_failure")) return 1;

  const std::string return_widening_success = R"(
    fn main() -> T81Float {
      return 7;
    }
  )";
  if (!expect_semantic_success(return_widening_success, "return_widening_success")) return 1;

  const std::string return_narrowing_failure = R"(
    fn main() -> i32 {
      return 1.5;
    }
  )";
  if (!expect_semantic_failure(return_narrowing_failure, "return_narrowing_failure")) return 1;

  std::cout << "semantic_analyzer_stage3_rules_test passed!\n";
  return 0;
}
