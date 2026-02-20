#include <iostream>
#include <memory>
#include <string_view>
#include <vector>
#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"

using namespace t81::frontend;

bool check_token(const Token& t, TokenType type, std::string_view lexeme) {
  if (t.type != type) {
    std::cerr << "Token type mismatch: expected " << (int)type << ", got " << (int)t.type << "\n";
    return false;
  }
  if (t.lexeme != lexeme) {
    std::cerr << "Token lexeme mismatch: expected " << lexeme << ", got " << t.lexeme << "\n";
    return false;
  }
  return true;
}

bool test_lexer() {
  const char* source = ":mysym \xE2\x88\x9E{seed} recurse distributed infinite";
  Lexer lexer(source);
  auto tokens = lexer.all_tokens();

  if (tokens.size() != 9) {
    std::cerr << "Lexer token count mismatch: " << tokens.size() << "\n";
    return false;
  }

  if (!check_token(tokens[0], TokenType::Symbol, ":mysym")) return false;
  if (tokens[1].type != TokenType::InfiniteLiteral) return false;
  if (tokens[1].lexeme.size() != 3) return false;

  if (!check_token(tokens[2], TokenType::LBrace, "{")) return false;
  if (!check_token(tokens[3], TokenType::Identifier, "seed")) return false;
  if (!check_token(tokens[4], TokenType::RBrace, "}")) return false;
  if (!check_token(tokens[5], TokenType::Recurse, "recurse")) return false;
  if (!check_token(tokens[6], TokenType::Distributed, "distributed")) return false;
  if (!check_token(tokens[7], TokenType::Infinite, "infinite")) return false;

  return true;
}

bool test_parser() {
  {
    const char* source = "let x = :foo;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();
    if (stmts.empty()) return false;
  }
  {
    const char* source = "recurse f(n) { base -> 1; step -> n; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();
    if (stmts.empty()) return false;
  }
  {
    const char* source = "distributed { let x = 1; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();
    if (stmts.empty()) return false;
  }
  {
    const char* source = "infinite { let x = 1; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();
    if (stmts.empty()) return false;
  }
  {
    const char* source = "let i = \xE2\x88\x9E{0};";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();
    if (stmts.empty()) return false;
  }
  return true;
}

bool test_semantic() {
  // Debug Type Equality
  Type t1{Type::Kind::Symbol};
  Type t2{Type::Kind::Symbol};
  if (t1 != t2) {
    std::cerr << "Type::operator== failed for Symbol kind.\n";
    return false;
  }

  {
    const char* source = "let x: Symbol = :foo;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();
    SemanticAnalyzer analyzer(stmts);
    analyzer.analyze();
    if (analyzer.had_error()) {
      std::cerr << "Semantic error in symbol assignment:\n";
      for (const auto& d : analyzer.diagnostics()) std::cerr << d.message << "\n";
      // Dump type of x and :foo?
      // Cannot easily access internal state.
      return false;
    }
  }
  {
    const char* source = "let x: InfiniteCanonicalForm = \xE2\x88\x9E{0};";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();
    SemanticAnalyzer analyzer(stmts);
    analyzer.analyze();
    if (analyzer.had_error()) {
      std::cerr << "Semantic error in infinite literal assignment:\n";
      for (const auto& d : analyzer.diagnostics()) std::cerr << d.message << "\n";
      return false;
    }
  }
  return true;
}

int main() {
  if (!test_lexer()) {
    std::cerr << "Lexer test failed\n";
    return 1;
  }
  if (!test_parser()) {
    std::cerr << "Parser test failed\n";
    return 1;
  }
  if (!test_semantic()) {
    std::cerr << "Semantic test failed\n";
    return 1;
  }
  std::cout << "Spec compliance tests passed!\n";
  return 0;
}
