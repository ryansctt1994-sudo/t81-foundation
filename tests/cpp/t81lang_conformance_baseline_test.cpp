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

static bool fails_semantic(std::string_view source,
                           const char* label = "t81lang_conformance_failure") {
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
  require_true(analyzes(source, "t81lang_tier_annotation_supported"),
               "t81lang_tier_annotation_supported");
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

static void test_t81_mixed_numeric_widening_matrix() {
  constexpr const char* bigint_float = R"(
    fn main() -> T81Float {
      let i: T81BigInt = 3t81;
      let f: T81Float = 1.5t81;
      let out: T81Float = i * f;
      return out;
    }
  )";
  require_true(analyzes(bigint_float, "t81lang_widen_bigint_float"), "t81lang_widen_bigint_float");

  constexpr const char* bigint_fraction = R"(
    fn main() -> T81Fraction {
      let i: T81BigInt = 2t81;
      let q: T81Fraction = 22/7t81;
      let out: T81Fraction = i + q;
      return out;
    }
  )";
  require_true(analyzes(bigint_fraction, "t81lang_widen_bigint_fraction"),
               "t81lang_widen_bigint_fraction");
}

static void test_base81_integer_infers_bigint() {
  constexpr const char* source = R"(
    fn main() -> T81BigInt {
      let x = 12t81;
      let y: T81BigInt = x;
      return y;
    }
  )";
  require_true(analyzes(source, "t81lang_base81_integer_infers_bigint"),
               "t81lang_base81_integer_infers_bigint");
}

static void test_base81_integer_does_not_silently_narrow_to_i32() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      let x = 12t81;
      let y: i32 = x;
      return y;
    }
  )";
  require_true(fails_semantic(source, "t81lang_base81_integer_no_narrow"),
               "t81lang_base81_integer_no_narrow");
}

static void test_t81_numeric_type_separation_rejects_invalid_mix() {
  constexpr const char* source = R"(
    fn main() -> i8 {
      let x: i2 = 1;
      return x + 1.5;
    }
  )";
  require_true(fails_semantic(source, "t81lang_numeric_type_separation"),
               "t81lang_numeric_type_separation");
}

static void test_base81_fraction_literal_is_native_t81fraction() {
  constexpr const char* source = R"(
    fn main() -> T81Fraction {
      let x = 22/7t81;
      let y: T81Fraction = x;
      return y;
    }
  )";
  require_true(analyzes(source, "t81lang_base81_fraction_native"),
               "t81lang_base81_fraction_native");
}

static void test_base81_fraction_literal_does_not_silently_narrow() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      let x = 22/7t81;
      let y: i32 = x;
      return y;
    }
  )";
  require_true(fails_semantic(source, "t81lang_base81_fraction_no_narrow"),
               "t81lang_base81_fraction_no_narrow");
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
  require_true(analyzes(source, "t81lang_print_builtin_numerics"),
               "t81lang_print_builtin_numerics");
}

static void test_print_builtin_rejects_bad_arity() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      print();
      return 0;
    }
  )";
  require_true(fails_semantic(source, "t81lang_print_builtin_bad_arity"),
               "t81lang_print_builtin_bad_arity");
}

static void test_std_namespace_builtin_aliases() {
  constexpr const char* source = R"(
    fn main() -> i32 {
      let angle: T81Float = std.math.sin(1.0);
      let c: T81Float = std.math.cos(angle);
      let t: T81Float = std.math.tan(c);
      std.io.println("ok");
      std.io.print_int(7);
      std.io.print_float(t);
      let handle: i32 = std.tensor.load("layer0.weight");
      return 0;
    }
  )";
  require_true(analyzes(source, "t81lang_std_namespace_builtin_aliases"),
               "t81lang_std_namespace_builtin_aliases");

  constexpr const char* bad_print_arity = R"(
    fn main() -> i32 {
      std.io.print_int();
      return 0;
    }
  )";
  require_true(fails_semantic(bad_print_arity, "t81lang_std_print_int_bad_arity"),
               "t81lang_std_print_int_bad_arity");

  constexpr const char* bad_math_type = R"(
    fn main() -> i32 {
      let x: T81Float = std.math.cos("oops");
      let _ = x;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_math_type, "t81lang_std_math_cos_bad_type"),
               "t81lang_std_math_cos_bad_type");
}

static void test_std_tensor_from_list_alias() {
  constexpr const char* valid = R"(
    fn main() -> i32 {
      let t: Tensor = std.tensor.from_list([1, 2, 3]);
      let _ = t;
      return 0;
    }
  )";
  require_true(analyzes(valid, "t81lang_std_tensor_from_list_alias"),
               "t81lang_std_tensor_from_list_alias");

  constexpr const char* bad_arity = R"(
    fn main() -> i32 {
      let t: Tensor = std.tensor.from_list();
      let _ = t;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_arity, "t81lang_std_tensor_from_list_bad_arity"),
               "t81lang_std_tensor_from_list_bad_arity");

  constexpr const char* bad_arg = R"(
    fn main() -> i32 {
      let t: Tensor = std.tensor.from_list("abc");
      let _ = t;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_arg, "t81lang_std_tensor_from_list_bad_arg"),
               "t81lang_std_tensor_from_list_bad_arg");
}

static void test_std_tensor_matmul_alias() {
  constexpr const char* valid = R"(
    fn main() -> i32 {
      let a: Tensor = std.tensor.from_list([1, 2, 3]);
      let b: Tensor = std.tensor.from_list([4, 5, 6]);
      let c: Tensor = std.tensor.matmul(a, b);
      let _ = c;
      return 0;
    }
  )";
  require_true(analyzes(valid, "t81lang_std_tensor_matmul_alias"),
               "t81lang_std_tensor_matmul_alias");

  constexpr const char* bad_arity = R"(
    fn main() -> i32 {
      let a: Tensor = std.tensor.from_list([1, 2, 3]);
      let c: Tensor = std.tensor.matmul(a);
      let _ = c;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_arity, "t81lang_std_tensor_matmul_bad_arity"),
               "t81lang_std_tensor_matmul_bad_arity");
}

static void test_std_tensor_vec_add_alias() {
  constexpr const char* valid = R"(
    fn main() -> i32 {
      let a: Tensor = std.tensor.from_list([1, 2, 3]);
      let b: Tensor = std.tensor.from_list([4, 5, 6]);
      let c: Tensor = std.tensor.vec_add(a, b);
      let _ = c;
      return 0;
    }
  )";
  require_true(analyzes(valid, "t81lang_std_tensor_vec_add_alias"),
               "t81lang_std_tensor_vec_add_alias");

  constexpr const char* bad_arity = R"(
    fn main() -> i32 {
      let a: Tensor = std.tensor.from_list([1, 2, 3]);
      let c: Tensor = std.tensor.vec_add(a);
      let _ = c;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_arity, "t81lang_std_tensor_vec_add_bad_arity"),
               "t81lang_std_tensor_vec_add_bad_arity");
}

static void test_std_text_aliases() {
  constexpr const char* valid = R"(
    fn main() -> i32 {
      let n: i32 = std.text.str_len("alpha");
      let e: bool = std.text.str_is_empty("");
      let c: T81String = std.text.concat("al", "pha");
      let sw: bool = std.text.starts_with(c, "al");
      let ew: bool = std.text.ends_with(c, "ha");
      let has_mid: bool = std.text.contains(c, "lp");
      let idx: i32 = std.text.index_of(c, "ph");
      let repl: T81String = std.text.replace(c, "ph", "zz");
      let repl_idx: i32 = std.text.index_of(repl, "zz");
      if (e) {
        if (sw) {
          if (ew) {
            if (has_mid) {
              if (idx == 2) {
                if (repl_idx == 2) {
                  return n;
                }
              }
            }
          }
        }
      }
      return 0;
    }
  )";
  require_true(analyzes(valid, "t81lang_std_text_aliases"), "t81lang_std_text_aliases");

  constexpr const char* bad_type = R"(
    fn main() -> i32 {
      let n: i32 = std.text.str_len(7);
      return n;
    }
  )";
  require_true(fails_semantic(bad_type, "t81lang_std_text_str_len_bad_type"),
               "t81lang_std_text_str_len_bad_type");

  constexpr const char* bad_arity = R"(
    fn main() -> i32 {
      let n: i32 = std.text.str_is_empty();
      return n;
    }
  )";
  require_true(fails_semantic(bad_arity, "t81lang_std_text_str_is_empty_bad_arity"),
               "t81lang_std_text_str_is_empty_bad_arity");

  constexpr const char* bad_concat_type = R"(
    fn main() -> i32 {
      let c: T81String = std.text.concat("alpha", 7);
      let _ = c;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_concat_type, "t81lang_std_text_concat_bad_type"),
               "t81lang_std_text_concat_bad_type");

  constexpr const char* bad_starts_with_arity = R"(
    fn main() -> i32 {
      let b: bool = std.text.starts_with("alpha");
      let _ = b;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_starts_with_arity, "t81lang_std_text_starts_with_bad_arity"),
               "t81lang_std_text_starts_with_bad_arity");

  constexpr const char* bad_ends_with_type = R"(
    fn main() -> i32 {
      let b: bool = std.text.ends_with(7, "a");
      let _ = b;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_ends_with_type, "t81lang_std_text_ends_with_bad_type"),
               "t81lang_std_text_ends_with_bad_type");

  constexpr const char* bad_contains_arity = R"(
    fn main() -> i32 {
      let b: bool = std.text.contains("alpha");
      let _ = b;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_contains_arity, "t81lang_std_text_contains_bad_arity"),
               "t81lang_std_text_contains_bad_arity");

  constexpr const char* bad_index_of_type = R"(
    fn main() -> i32 {
      let i: i32 = std.text.index_of("alpha", 7);
      let _ = i;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_index_of_type, "t81lang_std_text_index_of_bad_type"),
               "t81lang_std_text_index_of_bad_type");

  constexpr const char* bad_replace_arity = R"(
    fn main() -> i32 {
      let s: T81String = std.text.replace("alpha", "a");
      let _ = s;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_replace_arity, "t81lang_std_text_replace_bad_arity"),
               "t81lang_std_text_replace_bad_arity");

  constexpr const char* bad_replace_type = R"(
    fn main() -> i32 {
      let s: T81String = std.text.replace("alpha", 7, "b");
      let _ = s;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_replace_type, "t81lang_std_text_replace_bad_type"),
               "t81lang_std_text_replace_bad_type");
}

static void test_std_text_module_wrappers() {
  constexpr const char* source = R"(
    fn str_len(s: T81String) -> i32 {
      return std.text.str_len(s);
    }
    fn str_is_empty(s: T81String) -> bool {
      return std.text.str_is_empty(s);
    }
    fn concat(a: T81String, b: T81String) -> T81String {
      return std.text.concat(a, b);
    }
    fn starts_with(s: T81String, prefix: T81String) -> bool {
      return std.text.starts_with(s, prefix);
    }
    fn ends_with(s: T81String, suffix: T81String) -> bool {
      return std.text.ends_with(s, suffix);
    }
    fn contains(s: T81String, needle: T81String) -> bool {
      return std.text.contains(s, needle);
    }
    fn index_of(s: T81String, needle: T81String) -> i32 {
      return std.text.index_of(s, needle);
    }
    fn replace(s: T81String, needle: T81String, replacement: T81String) -> T81String {
      return std.text.replace(s, needle, replacement);
    }
    fn main() -> i32 {
      let joined: T81String = concat("ze", "ta");
      let n: i32 = str_len(joined);
      let e: bool = str_is_empty("");
      let sw: bool = starts_with(joined, "ze");
      let ew: bool = ends_with(joined, "ta");
      let has_mid: bool = contains(joined, "et");
      let idx: i32 = index_of(joined, "ta");
      let replaced: T81String = replace(joined, "ta", "xo");
      let repl_idx: i32 = index_of(replaced, "xo");
      if (e) {
        if (sw) {
          if (ew) {
            if (has_mid) {
              if (idx == 2) {
                if (repl_idx == 2) {
                  return n;
                }
              }
            }
          }
        }
      }
      return 0;
    }
  )";
  require_true(analyzes(source, "t81lang_std_text_module_wrappers"),
               "t81lang_std_text_module_wrappers");
}

static void test_std_bytes_aliases() {
  constexpr const char* valid = R"(
    fn main() -> i32 {
      let n: i32 = std.bytes.len("alpha");
      let e: bool = std.bytes.is_empty("");
      let c: T81String = std.bytes.concat("al", "pha");
      let sw: bool = std.bytes.starts_with(c, "al");
      let ew: bool = std.bytes.ends_with(c, "ha");
      let has_mid: bool = std.bytes.contains(c, "lp");
      let idx: i32 = std.bytes.index_of(c, "ph");
      let repl: T81String = std.bytes.replace(c, "ph", "zz");
      let repl_idx: i32 = std.bytes.index_of(repl, "zz");
      let m: i32 = std.bytes.len(c);
      if (e) {
        if (n == 5) {
          if (m == 5) {
            if (sw) {
              if (ew) {
                if (has_mid) {
                  if (idx == 2) {
                    if (repl_idx == 2) {
                      return n;
                    }
                  }
                }
              }
            }
          }
        }
      }
      return 0;
    }
  )";
  require_true(analyzes(valid, "t81lang_std_bytes_aliases"), "t81lang_std_bytes_aliases");

  constexpr const char* bad_type = R"(
    fn main() -> i32 {
      let n: i32 = std.bytes.len(7);
      return n;
    }
  )";
  require_true(fails_semantic(bad_type, "t81lang_std_bytes_len_bad_type"),
               "t81lang_std_bytes_len_bad_type");

  constexpr const char* bad_arity = R"(
    fn main() -> i32 {
      let n: i32 = std.bytes.is_empty();
      return n;
    }
  )";
  require_true(fails_semantic(bad_arity, "t81lang_std_bytes_is_empty_bad_arity"),
               "t81lang_std_bytes_is_empty_bad_arity");

  constexpr const char* bad_concat_type = R"(
    fn main() -> i32 {
      let c: T81String = std.bytes.concat("alpha", 7);
      let _ = c;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_concat_type, "t81lang_std_bytes_concat_bad_type"),
               "t81lang_std_bytes_concat_bad_type");

  constexpr const char* bad_starts_with_arity = R"(
    fn main() -> i32 {
      let b: bool = std.bytes.starts_with("alpha");
      let _ = b;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_starts_with_arity, "t81lang_std_bytes_starts_with_bad_arity"),
               "t81lang_std_bytes_starts_with_bad_arity");

  constexpr const char* bad_ends_with_type = R"(
    fn main() -> i32 {
      let b: bool = std.bytes.ends_with(7, "a");
      let _ = b;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_ends_with_type, "t81lang_std_bytes_ends_with_bad_type"),
               "t81lang_std_bytes_ends_with_bad_type");

  constexpr const char* bad_contains_arity = R"(
    fn main() -> i32 {
      let b: bool = std.bytes.contains("alpha");
      let _ = b;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_contains_arity, "t81lang_std_bytes_contains_bad_arity"),
               "t81lang_std_bytes_contains_bad_arity");

  constexpr const char* bad_index_of_type = R"(
    fn main() -> i32 {
      let i: i32 = std.bytes.index_of("alpha", 7);
      let _ = i;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_index_of_type, "t81lang_std_bytes_index_of_bad_type"),
               "t81lang_std_bytes_index_of_bad_type");

  constexpr const char* bad_replace_arity = R"(
    fn main() -> i32 {
      let s: T81String = std.bytes.replace("alpha", "a");
      let _ = s;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_replace_arity, "t81lang_std_bytes_replace_bad_arity"),
               "t81lang_std_bytes_replace_bad_arity");

  constexpr const char* bad_replace_type = R"(
    fn main() -> i32 {
      let s: T81String = std.bytes.replace("alpha", 7, "b");
      let _ = s;
      return 0;
    }
  )";
  require_true(fails_semantic(bad_replace_type, "t81lang_std_bytes_replace_bad_type"),
               "t81lang_std_bytes_replace_bad_type");
}

static void test_std_bytes_module_wrappers() {
  constexpr const char* source = R"(
    fn len(b: T81String) -> i32 {
      return std.bytes.len(b);
    }
    fn is_empty(b: T81String) -> bool {
      return std.bytes.is_empty(b);
    }
    fn concat(a: T81String, b: T81String) -> T81String {
      return std.bytes.concat(a, b);
    }
    fn starts_with(b: T81String, prefix: T81String) -> bool {
      return std.bytes.starts_with(b, prefix);
    }
    fn ends_with(b: T81String, suffix: T81String) -> bool {
      return std.bytes.ends_with(b, suffix);
    }
    fn contains(b: T81String, needle: T81String) -> bool {
      return std.bytes.contains(b, needle);
    }
    fn index_of(b: T81String, needle: T81String) -> i32 {
      return std.bytes.index_of(b, needle);
    }
    fn replace(b: T81String, needle: T81String, replacement: T81String) -> T81String {
      return std.bytes.replace(b, needle, replacement);
    }
    fn main() -> i32 {
      let merged: T81String = concat("ze", "ta");
      let n: i32 = len(merged);
      let e: bool = is_empty("");
      let sw: bool = starts_with(merged, "ze");
      let ew: bool = ends_with(merged, "ta");
      let has_mid: bool = contains(merged, "et");
      let idx: i32 = index_of(merged, "ta");
      let replaced: T81String = replace(merged, "ta", "xo");
      let repl_idx: i32 = index_of(replaced, "xo");
      let m: i32 = len(merged);
      if (e) {
        if (n == 4) {
          if (m == 4) {
            if (sw) {
              if (ew) {
                if (has_mid) {
                  if (idx == 2) {
                    if (repl_idx == 2) {
                      return n;
                    }
                  }
                }
              }
            }
          }
        }
      }
      return 0;
    }
  )";
  require_true(analyzes(source, "t81lang_std_bytes_module_wrappers"),
               "t81lang_std_bytes_module_wrappers");
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
  test_t81_mixed_numeric_widening_matrix();
  test_base81_integer_infers_bigint();
  test_base81_integer_does_not_silently_narrow_to_i32();
  test_base81_fraction_literal_is_native_t81fraction();
  test_base81_fraction_literal_does_not_silently_narrow();
  test_print_builtin_accepts_native_t81_numerics();
  test_print_builtin_rejects_bad_arity();
  test_std_namespace_builtin_aliases();
  test_std_tensor_from_list_alias();
  test_std_tensor_matmul_alias();
  test_std_tensor_vec_add_alias();
  test_std_text_aliases();
  test_std_text_module_wrappers();
  test_std_bytes_aliases();
  test_std_bytes_module_wrappers();
  test_t81_numeric_type_separation_rejects_invalid_mix();
  test_let_is_immutable();
  test_var_is_mutable();
  std::cout << "t81lang conformance baseline tests passed!\n";
  return 0;
}
