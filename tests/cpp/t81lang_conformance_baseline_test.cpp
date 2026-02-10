#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace t81::frontend;

static bool analyzes(std::string_view source, const char* label = "t81lang_conformance_case") {
  std::string source_text(source);
  Lexer lexer{source_text};
  Parser parser(lexer, label);
  auto stmts = parser.parse();
  if (parser.had_error()) return false;
  SemanticAnalyzer analyzer(stmts, label);
  analyzer.analyze();
  return !analyzer.had_error();
}

static bool fails_semantic(std::string_view source, const char* label = "t81lang_conformance_failure") {
  std::string source_text(source);
  Lexer lexer{source_text};
  Parser parser(lexer, label);
  auto stmts = parser.parse();
  if (parser.had_error()) return false;
  SemanticAnalyzer analyzer(stmts, label);
  analyzer.analyze();
  return analyzer.had_error();
}

static void require_true(bool condition, const char* label) {
  if (!condition) {
    std::cerr << "Conformance assertion failed: " << label << "\n";
    std::abort();
  }
}

static void test_baseline_supported_features() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      let maybe: Option[i32] = Some(1);
      let v: i32 = match (maybe) {
        Some(x) => x;
        None => 0;
      };
      return v;
    }
  )";
  require_true(analyzes(source, "t81lang_supported_baseline"), "t81lang_supported_baseline");
}

static void test_tier_annotation_supported_for_functions() {
  constexpr const char* source = R"(
    @tier(2)
    fn main() -> i32 {
      return 0;
    }
  )";
  require_true(analyzes(source, "t81lang_tier_annotation_supported"), "t81lang_tier_annotation_supported");
}

static void test_t81_numeric_types_bind_and_widen() {
  constexpr const char* source = R"(
    fn main() -> T81Float {
      let a: T81Float = 1.20t81;
      let b: T81BigInt = 12t81;
      let c: T81Float = a + b;
      return c;
    }
  )";
  require_true(analyzes(source, "t81lang_numeric_binding"), "t81lang_numeric_binding");
}

static void test_base81_integer_infers_bigint() {
  constexpr const char* source = R"(
    fn main() -> T81BigInt {
      let x = 12t81;
      let y: T81BigInt = x;
      return y;
    }
  )";
  require_true(analyzes(source, "t81lang_base81_integer_infers_bigint"), "t81lang_base81_integer_infers_bigint");
}

static void test_base81_integer_does_not_silently_narrow_to_i32() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      let x = 12t81;
      let y: i32 = x;
      return y;
    }
  )";
  require_true(fails_semantic(source, "t81lang_base81_integer_no_narrow"), "t81lang_base81_integer_no_narrow");
}

static void test_t81_numeric_type_separation_rejects_invalid_mix() {
  constexpr const char* source = R"(
    fn main() -> i8 {
      let x: i2 = 1;
      return x + 1.5;
    }
  )";
  require_true(fails_semantic(source, "t81lang_numeric_type_separation"), "t81lang_numeric_type_separation");
}

static void test_base81_fraction_literal_is_native_t81fraction() {
  constexpr const char* source = R"(
    fn main() -> T81Fraction {
      let x = 22/7t81;
      let y: T81Fraction = x;
      return y;
    }
  )";
  require_true(analyzes(source, "t81lang_base81_fraction_native"), "t81lang_base81_fraction_native");
}

static void test_base81_fraction_literal_does_not_silently_narrow() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      let x = 22/7t81;
      let y: i32 = x;
      return y;
    }
  )";
  require_true(fails_semantic(source, "t81lang_base81_fraction_no_narrow"), "t81lang_base81_fraction_no_narrow");
}

static void test_print_builtin_accepts_native_t81_numerics() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      let i: T81BigInt = 12t81;
      let f: T81Float = 1.20t81;
      let q: T81Fraction = 22/7t81;
      print(i);
      print(f);
      print(q);
      return 0;
    }
  )";
  require_true(analyzes(source, "t81lang_print_builtin_numerics"), "t81lang_print_builtin_numerics");
}

static void test_print_builtin_rejects_bad_arity() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      print();
      return 0;
    }
  )";
  require_true(fails_semantic(source, "t81lang_print_builtin_bad_arity"), "t81lang_print_builtin_bad_arity");
}

static void test_let_is_immutable() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      let x: i32 = 1;
      x = 2;
      return x;
    }
  )";
  require_true(fails_semantic(source, "t81lang_let_immutable"), "t81lang_let_immutable");
}

static void test_var_is_mutable() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      var x: i32 = 1;
      x = 2;
      return x;
    }
  )";
  require_true(analyzes(source, "t81lang_var_mutable"), "t81lang_var_mutable");
}

int main() {
  test_baseline_supported_features();
  test_tier_annotation_supported_for_functions();
  test_t81_numeric_types_bind_and_widen();
  test_base81_integer_infers_bigint();
  test_base81_integer_does_not_silently_narrow_to_i32();
  test_base81_fraction_literal_is_native_t81fraction();
  test_base81_fraction_literal_does_not_silently_narrow();
  test_print_builtin_accepts_native_t81_numerics();
  test_print_builtin_rejects_bad_arity();
  test_t81_numeric_type_separation_rejects_invalid_mix();
  test_let_is_immutable();
  test_var_is_mutable();
  std::cout << "t81lang conformance baseline tests passed!\n";
  return 0;
}
