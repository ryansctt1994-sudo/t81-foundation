#include "t81/frontend/ir_generator.hpp"
#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include "t81/tisc/binary_emitter.hpp"
#include "t81/vm/vm.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace t81;

static std::vector<std::string> run_and_capture_prints(const std::string& source) {
  frontend::Lexer lexer(source);
  frontend::Parser parser(lexer);
  auto stmts = parser.parse();
  assert(!parser.had_error());

  frontend::SemanticAnalyzer analyzer(stmts);
  analyzer.analyze();
  assert(!analyzer.had_error());

  frontend::IRGenerator ir_gen;
  ir_gen.attach_semantic_analyzer(&analyzer);
  tisc::ir::IntermediateProgram ir = ir_gen.generate(stmts);

  tisc::BinaryEmitter emitter;
  tisc::Program program = emitter.emit(ir);

  auto vm = vm::make_interpreter_vm();
  vm->load_program(program);
  [[maybe_unused]] auto run = vm->run_to_halt();
  assert(run.has_value());

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

int main() {
  test_print_runtime_captures_scalars_in_order();
  std::cout << "e2e print runtime test passed!\n";
  return 0;
}
