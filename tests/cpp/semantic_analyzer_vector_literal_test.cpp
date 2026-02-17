#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"

#include <iostream>
#include <string>

using namespace t81::frontend;

bool expect_semantic_success(const std::string& source, const char* label) {
  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();
  if (parser.had_error()) {
    std::cerr << "semantic_analyzer_vector_literal_test parser failure in fixture: " << label
              << "\n";
    return false;
  }

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  if (analyzer.had_error()) {
    std::cerr << "semantic_analyzer_vector_literal_test semantic failure in fixture: " << label
              << "\n";
    for (const auto& diag : analyzer.diagnostics()) {
      std::cerr << "  line " << diag.line << ":" << diag.column << " " << diag.message << "\n";
    }
    return false;
  }
  return true;
}

bool expect_semantic_failure(const std::string& source, const char* label) {
  Lexer lexer(source);
  Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();
  if (parser.had_error()) return true;

  SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  if (!analyzer.had_error()) {
    std::cerr << "semantic_analyzer_vector_literal_test expected failure fixture passed: " << label
              << "\n";
    return false;
  }
  return true;
}

int main() {
  const std::string simple_vector = R"(
        fn main() -> i32 {
            let v = [1, 2, 3];
            return 0;
        }
    )";
  if (!expect_semantic_success(simple_vector, "simple_vector")) return 1;

  const std::string float_vector = R"(
        fn main() -> i32 {
            let v = [1, 2.5];
            return 0;
        }
    )";
  if (!expect_semantic_success(float_vector, "float_vector")) return 1;

  const std::string no_context = R"(
        fn main() -> i32 {
            [[maybe_unused]] let v= [];
            return 0;
        }
    )";
  if (!expect_semantic_failure(no_context, "no_context")) return 1;

  std::cout << "Semantic analyzer vector literal tests passed!" << std::endl;
  return 0;
}
