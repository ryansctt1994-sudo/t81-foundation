#include <iostream>
#include <string>

#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"

using namespace t81::frontend;

namespace {

bool expect(bool cond, const char* msg) {
  if (!cond) {
    std::cerr << "semantic_analyzer_cascade_suppression_test failure: " << msg << "\n";
    return false;
  }
  return true;
}

bool has_message(const std::vector<Diagnostic>& diagnostics, std::string_view text) {
  for (const auto& diag : diagnostics) {
    if (diag.message.find(text) != std::string::npos) {
      return true;
    }
  }
  return false;
}

int count_message(const std::vector<Diagnostic>& diagnostics, std::string_view text) {
  int count = 0;
  for (const auto& diag : diagnostics) {
    if (diag.message.find(text) != std::string::npos) {
      ++count;
    }
  }
  return count;
}

}  // namespace

int main() {
  const std::string source = R"(
fn main() -> i32 {
  missing.field = 1;
  return 0;
}
)";

  Lexer lexer(source);
  Parser parser(lexer, "cascade-suppression");
  auto stmts = parser.parse();
  if (!expect(!parser.had_error(), "unexpected parser error")) return 1;

  SemanticAnalyzer analyzer(stmts, "cascade-suppression");
  analyzer.analyze();
  if (!expect(analyzer.had_error(), "expected semantic error did not occur")) return 1;

  const auto& diagnostics = analyzer.diagnostics();
  if (!expect(has_message(diagnostics, "Undefined variable 'missing'"),
              "missing root-cause undefined-variable diagnostic")) {
    return 1;
  }

  if (!expect(!has_message(diagnostics, "Field access requires a record value."),
              "field-access follow-on diagnostic should be suppressed")) {
    return 1;
  }

  if (!expect(!has_message(diagnostics, "Cannot assign to immutable field."),
              "immutable-field follow-on diagnostic should be suppressed")) {
    return 1;
  }

  if (!expect(count_message(diagnostics, "Undefined variable 'missing'") == 1,
              "undefined-variable diagnostic should not be duplicated")) {
    return 1;
  }

  return 0;
}
