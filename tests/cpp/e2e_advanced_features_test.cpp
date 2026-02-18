#include "t81/frontend/ir_generator.hpp"
#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include "t81/tisc/binary_emitter.hpp"
#include "t81/vm/state.hpp"
#include "t81/vm/vm.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace t81;

// Helper to compile and run a T81Lang source string and return the final value of register r0.
int64_t run_e2e_test(const std::string& source) {
  frontend::Lexer lexer(source);
  frontend::Parser parser(lexer);
  [[maybe_unused]] auto stmts = parser.parse();
  if (parser.had_error()) {
    std::cerr << "Parser error!" << std::endl;
    return -1;
  }

  frontend::SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  if (analyzer.had_error()) {
    std::cerr << "Semantic Analyzer error!" << std::endl;
    for (const auto& diag : analyzer.diagnostics()) {
      std::cerr << diag.line << ":" << diag.column << ": " << diag.message << std::endl;
    }
    return -2;
  }

  [[maybe_unused]] frontend::IRGenerator ir_gen;
  ir_gen.attach_semantic_analyzer(&analyzer);
  [[maybe_unused]] tisc::ir::IntermediateProgram ir = ir_gen.generate(stmts);

  [[maybe_unused]] tisc::BinaryEmitter emitter;
  [[maybe_unused]] tisc::Program program = emitter.emit(ir);

  [[maybe_unused]] auto vm = vm::make_interpreter_vm();
  vm->load_program(program);
  vm->run_to_halt();

  // Return R2 as it holds the return value of main
  return vm->state().registers[2];
}

void test_while_break() {
  const std::string source = R"(
        fn main() -> i32 {
            var i: i32 = 0;
            while (1 == 1) {
                if (i == 10) {
                    break;
                }
                i = i + 1;
            }
            return i;
        }
    )";
  [[maybe_unused]] int64_t result = run_e2e_test(source);
  if (result != 10) {
    std::cerr << "test_while_break failed: expected 10, got " << result << std::endl;
    throw std::runtime_error("test_while_break failed");
  }
}

void test_nested_loop_continue() {
  const std::string source = R"(
        fn main() -> i32 {
            var sum: i32 = 0;
            var i: i32 = 0;
            @bounded(10)
            loop {
                i = i + 1;
                if (i > 9) {
                    break;
                }
                if (i % 2 == 0) {
                    continue;
                }
                sum = sum + i;
            }
            return sum; // 1 + 3 + 5 + 7 + 9 = 25
        }
    )";
  [[maybe_unused]] int64_t result = run_e2e_test(source);
  if (result != 25) {
    std::cerr << "test_nested_loop_continue failed: expected 25, got " << result << std::endl;
    throw std::runtime_error("test_nested_loop_continue failed");
  }
}

void test_match_guards() {
  const std::string source = R"(
        fn main() -> i32 {
            let x: i32 = 5;
            let opt: Option[i32] = Some(x);
            let result: i32 = match (opt) {
                Some(v) if v > 10 => 100,
                Some(v) if v < 10 => 200,
                Some(v) => 300,
                None => 0
            };
            return result; // 200
        }
    )";
  [[maybe_unused]] int64_t result = run_e2e_test(source);
  if (result != 200) {
    std::cerr << "test_match_guards failed: expected 200, got " << result << std::endl;
    throw std::runtime_error("test_match_guards failed");
  }
}

void test_custom_enum_match() {
  const std::string source = R"(
        enum Status {
            Idle;
            Active(i32);
            Error(i32);
        };

        fn main() -> i32 {
            let s: Status = Status.Active(42);
            return match (s) {
                Idle => 1,
                Active(v) => v,
                Error(e) => 0 - e
            };
        }
    )";
  [[maybe_unused]] int64_t result = run_e2e_test(source);
  if (result != 42) {
    std::cerr << "test_custom_enum_match failed: expected 42, got " << result << std::endl;
    throw std::runtime_error("test_custom_enum_match failed");
  }
}

void test_extended_numeric_types_pipeline() {
  const std::string source = R"(
        fn main() -> T81Uint {
            let q: T81Qutrit = 1;
            let q2: T81Qutrit = q + 1;
            let u: T81Uint = 7;
            let u2: T81Uint = u + 2;
            let f: T81Fixed[8, 4] = 3;
            let _f2: T81Fixed[8, 4] = f + 4;
            return q2 + u2;
        }
    )";
  [[maybe_unused]] int64_t result = run_e2e_test(source);
  if (result != 11) {
    std::cerr << "test_extended_numeric_types_pipeline failed: expected 11, got " << result
              << std::endl;
    throw std::runtime_error("test_extended_numeric_types_pipeline failed");
  }
}

void test_constructor_and_conversion_pipeline() {
  const std::string source = R"(
        fn main() -> T81Uint {
            let q: T81Qutrit = T81Qutrit(1);
            let u: T81Uint = T81Uint(3);
            let f: T81Fixed[8, 4] = T81Fixed[8, 4](u);
            return u + 1;
        }
    )";
  [[maybe_unused]] int64_t result = run_e2e_test(source);
  if (result != 4) {
    std::cerr << "test_constructor_and_conversion_pipeline failed: expected 4, got " << result
              << std::endl;
    throw std::runtime_error("test_constructor_and_conversion_pipeline failed");
  }
}

void test_complex_constructor_pipeline() {
  const std::string source = R"(
        fn main() -> i32 {
            let _c: T81Complex[18] = T81Complex[18](1, -1);
            return 7;
        }
    )";
  [[maybe_unused]] int64_t result = run_e2e_test(source);
  if (result != 7) {
    std::cerr << "test_complex_constructor_pipeline failed: expected 7, got " << result
              << std::endl;
    throw std::runtime_error("test_complex_constructor_pipeline failed");
  }
}

void test_std_tensor_from_list_pipeline() {
  const std::string source = R"(
        fn main() -> i32 {
            let t: Tensor = std.tensor.from_list([1, 2, 3]);
            let _ = t;
            return 17;
        }
    )";
  [[maybe_unused]] int64_t result = run_e2e_test(source);
  if (result != 17) {
    std::cerr << "test_std_tensor_from_list_pipeline failed: expected 17, got " << result
              << std::endl;
    throw std::runtime_error("test_std_tensor_from_list_pipeline failed");
  }
}

void test_std_tensor_vec_add_pipeline() {
  const std::string source = R"(
        fn main() -> i32 {
            let a: Tensor = std.tensor.from_list([1, 2, 3]);
            let b: Tensor = std.tensor.from_list([4, 5, 6]);
            let c: Tensor = std.tensor.vec_add(a, b);
            let _ = c;
            return 19;
        }
    )";
  [[maybe_unused]] int64_t result = run_e2e_test(source);
  if (result != 19) {
    std::cerr << "test_std_tensor_vec_add_pipeline failed: expected 19, got " << result
              << std::endl;
    throw std::runtime_error("test_std_tensor_vec_add_pipeline failed");
  }
}

void test_std_text_pipeline() {
  const std::string source = R"(
        fn main() -> i32 {
            let n: i32 = std.text.str_len("alpha");
            let e: bool = std.text.str_is_empty("");
            let joined: T81String = std.text.concat("al", "pha");
            let sw: bool = std.text.starts_with(joined, "al");
            let ew: bool = std.text.ends_with(joined, "ha");
            let has_mid: bool = std.text.contains(joined, "lp");
            let idx: i32 = std.text.index_of(joined, "ph");
            let replaced: T81String = std.text.replace(joined, "ph", "zz");
            let repl_idx: i32 = std.text.index_of(replaced, "zz");
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
  [[maybe_unused]] int64_t result = run_e2e_test(source);
  if (result != 5) {
    std::cerr << "test_std_text_pipeline failed: expected 5, got " << result << std::endl;
    throw std::runtime_error("test_std_text_pipeline failed");
  }
}

void test_std_text_module_wrapper_pipeline() {
  const std::string source = R"(
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
            let joined: T81String = concat("om", "ega");
            let n: i32 = str_len(joined);
            let e: bool = str_is_empty("");
            let sw: bool = starts_with(joined, "om");
            let ew: bool = ends_with(joined, "ga");
            let has_mid: bool = contains(joined, "meg");
            let idx: i32 = index_of(joined, "ga");
            let replaced: T81String = replace(joined, "ga", "xy");
            let repl_idx: i32 = index_of(replaced, "xy");
            if (e) {
                if (sw) {
                    if (ew) {
                        if (has_mid) {
                            if (idx == 3) {
                                if (repl_idx == 3) {
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
  [[maybe_unused]] int64_t result = run_e2e_test(source);
  if (result != 5) {
    std::cerr << "test_std_text_module_wrapper_pipeline failed: expected 5, got " << result
              << std::endl;
    throw std::runtime_error("test_std_text_module_wrapper_pipeline failed");
  }
}

int main() {
  test_while_break();
  test_nested_loop_continue();
  test_match_guards();
  test_custom_enum_match();
  test_extended_numeric_types_pipeline();
  test_constructor_and_conversion_pipeline();
  test_complex_constructor_pipeline();
  test_std_tensor_from_list_pipeline();
  test_std_tensor_vec_add_pipeline();
  test_std_text_pipeline();
  test_std_text_module_wrapper_pipeline();
  std::cout << "All advanced E2E tests passed!" << std::endl;
  return 0;
}
