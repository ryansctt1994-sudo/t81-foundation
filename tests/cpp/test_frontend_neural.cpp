#include <iostream>
#include <string>
#include <vector>
#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "test_utils.hpp"

using namespace t81::frontend;

int main() {
  AstPrinter printer;

  // Test 1: infer expression
  {
    std::string source = "let result = infer model(input);";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();

    if (parser.had_error()) {
      std::cerr << "Infer parsing failed" << std::endl;
      return 1;
    }

    // Stmt 0 is LetStmt
    // Initializer is InferExpr
    std::string result = printer.print(*stmts[0]);
    // Expected: (let result = (infer (call model input)))
    std::string expected = "(let result = (infer (call model input)))";

    if (result != expected) {
      std::cerr << "Infer test failed!" << std::endl;
      std::cerr << "Expected: " << expected << std::endl;
      std::cerr << "Actual:   " << result << std::endl;
      return 1;
    }
    std::cout << "Infer test passed!" << std::endl;
  }

  // Test 2: train statement
  {
    std::string source = R"(
      train (model) {
        let loss = infer model(data);
      }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();

    if (parser.had_error()) {
      std::cerr << "Train parsing failed" << std::endl;
      return 1;
    }

    std::string result = printer.print(*stmts[0]);
    // Expected: (train model (block (let loss = (infer (call model data)))))
    std::string expected = "(train model (block (let loss = (infer (call model data)))))";

    if (result != expected) {
      std::cerr << "Train test failed!" << std::endl;
      std::cerr << "Expected: " << expected << std::endl;
      std::cerr << "Actual:   " << result << std::endl;
      return 1;
    }
    std::cout << "Train test passed!" << std::endl;
  }

  return 0;
}
