#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"

#include <iostream>
#include <string>

using namespace t81::frontend;

bool expect_semantic_success(const std::string& source, const char* label = "<success fixture>") {
  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "semantic_analyzer_numeric_test failure: " << msg << "\n";
      return false;
    }
    return true;
  };

  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();
  if (parser.had_error()) {
    std::cerr << "[" << label << "] parser reported errors\n";
  }
  if (!expect(!parser.had_error(), "unexpected parser error in success fixture")) return false;

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  if (!expect(!analyzer.had_error(), "unexpected semantic error in success fixture")) return false;
  return true;
}

bool expect_semantic_failure(const std::string& source, const char* label = "<failure fixture>") {
  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "semantic_analyzer_numeric_test failure: " << msg << "\n";
      return false;
    }
    return true;
  };

  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();
  if (parser.had_error()) {
    // Parsing already failed, acceptable for these fixtures.
    return true;
  }

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  if (!expect(analyzer.had_error(), "expected semantic failure did not occur")) return false;
  (void)label;
  return true;
}

int main() {
  const std::string float_fraction_failure = R"(
        fn main() -> T81Float {
            return 1.20t81 + 22/7t81;
        }
    )";
  if (!expect_semantic_failure(float_fraction_failure, "float_fraction_failure")) return 1;

  const std::string bigint_float_success = R"(
        fn main() -> T81Float {
            let big: T81BigInt = 123456;
            let result: T81Float = big + 1.20t81;
            return result;
        }
    )";
  if (!expect_semantic_success(bigint_float_success, "bigint_float_success")) return 1;

  std::cout << "Semantic analyzer numeric rules tests passed!" << std::endl;
  return 0;
}
