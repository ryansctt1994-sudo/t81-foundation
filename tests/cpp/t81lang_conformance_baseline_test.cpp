#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"

#include <cassert>
#include <iostream>
#include <string>

using namespace t81::frontend;

static bool analyzes(std::string_view source, const char* label = "t81lang_conformance_case") {
  Lexer lexer{std::string(source)};
  Parser parser(lexer, label);
  auto stmts = parser.parse();
  if (parser.had_error()) return false;
  SemanticAnalyzer analyzer(stmts, label);
  analyzer.analyze();
  return !analyzer.had_error();
}

static void test_baseline_supported_features() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      @bounded(loop(1 < 2))
      loop {
        let items: Vector[i32] = [1, 2, 3];
        let maybe: Option[i32] = Some(items[0]);
        let v: i32 = match (maybe) {
          Some(x) => x;
          None => 0;
        };
        return v;
      }
      return 0;
    }
  )";
  assert(analyzes(source, "t81lang_supported_baseline"));
}

static void test_known_gap_tier_annotation_rejected() {
  constexpr const char* source = R"(
    @tier(2)
    fn main() -> i32 {
      return 0;
    }
  )";
  // Current parser only supports @bounded(...) annotations for loops.
  assert(!analyzes(source, "t81lang_known_gap_tier_annotation"));
}

int main() {
  test_baseline_supported_features();
  test_known_gap_tier_annotation_rejected();
  std::cout << "t81lang conformance baseline tests passed!\n";
  return 0;
}
