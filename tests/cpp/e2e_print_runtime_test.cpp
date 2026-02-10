#include "t81/frontend/ir_generator.hpp"
#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include "t81/tisc/binary_emitter.hpp"
#include "t81/vm/vm.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace t81;

static std::vector<std::string> run_and_capture_prints(const std::string& source) {
  frontend::Lexer lexer(source);
  frontend::Parser parser(lexer);
  auto stmts = parser.parse();
  if (parser.had_error()) {
    throw std::runtime_error("Parser failed in run_and_capture_prints");
  }

  frontend::SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  if (analyzer.had_error()) {
    throw std::runtime_error("Semantic analysis failed in run_and_capture_prints");
  }

  frontend::IRGenerator ir_gen;
  ir_gen.attach_semantic_analyzer(&analyzer);
  tisc::ir::IntermediateProgram ir = ir_gen.generate(stmts);

  tisc::BinaryEmitter emitter;
  tisc::Program program = emitter.emit(ir);

  auto vm = vm::make_interpreter_vm();
  vm->load_program(program);
  auto run = vm->run_to_halt();
  if (!run.has_value()) {
    throw std::runtime_error("VM execution failed in run_and_capture_prints");
  }

  return vm->state().printed_output;
}

static void test_print_runtime_captures_scalars_in_order() {
  const std::string source = R"(
    fn main() -> i32 {
      let i: i32 = 42;
      let s: T81String = "alpha";
      print(i);
      print(s);
      print(7);
      return 0;
    }
  )";

  const auto output = run_and_capture_prints(source);
  assert(output.size() == 3);
  assert(output[0] == "42");
  assert(output[1] == "alpha");
  assert(output[2] == "7");
}

static void test_print_runtime_supports_t81_literals_and_bool() {
  const std::string source = R"(
    fn main() -> i32 {
      let bi: T81BigInt = 12t81;
      let tf: T81Float = 1.20t81;
      let fr: T81Fraction = 22/7t81;
      print(bi);
      print(tf);
      print(fr);
      print(true);
      print(false);
      return 0;
    }
  )";

  const auto output = run_and_capture_prints(source);
  assert(output.size() == 5);
  assert(output[0] == "12");
  assert(output[1].find("t81") != std::string::npos);
  assert(output[2] == "22/7t81");
  assert(output[3] == "true");
  assert(output[4] == "false");
}

static void test_runtime_arithmetic_for_t81_numeric_families() {
  const std::string source = R"(
    fn main() -> i32 {
      let i1: T81BigInt = 12t81;
      let i_two: T81BigInt = 5t81;
      let isum: T81BigInt = i1 + i_two;

      let f1: T81Float = 1;
      let f2: T81Float = 2;
      let fdiv: T81Float = f1 / f2;

      let q1: T81Fraction = 22/7t81;
      let q2: T81Fraction = 1/7t81;
      let qsub: T81Fraction = q1 - q2;

      print(isum);
      print(fdiv);
      print(qsub);
      return 0;
    }
  )";

  const auto output = run_and_capture_prints(source);
  assert(output.size() == 3);
  assert(output[0] == "17");
  assert(output[1] == "0.5t81");
  assert(output[2] == "3/1t81");
}

int main() {
  test_print_runtime_captures_scalars_in_order();
  test_print_runtime_supports_t81_literals_and_bool();
  test_runtime_arithmetic_for_t81_numeric_families();
  std::cout << "e2e print runtime test passed!\n";
  return 0;
}
