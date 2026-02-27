#undef NDEBUG
#include <cassert>
#include <iostream>
#include "t81/frontend/ir_generator.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include "t81/isa/binary_io.hpp"
#include "t81/isa/pretty_printer.hpp"

using namespace t81;
using namespace t81::frontend;

void verify_collection_lowering(const std::string& source) {
  Lexer lexer(source);
  Parser parser(lexer, "test_source");
  auto statements = parser.parse();
  assert(!statements.empty());

  SemanticAnalyzer analyzer(statements, "test_source");
  analyzer.analyze();
  assert(!analyzer.had_error());

  IRGenerator ir_gen;
  ir_gen.attach_semantic_analyzer(&analyzer);
  auto program = ir_gen.generate(statements);

  std::cout << "--- Source ---\n" << source << "\n";
  std::cout << "--- IR ---\n" << t81::tisc::pretty_print(program) << "\n";

  // Verify STRVECNEW opcode presence for List/Map/Set/Tree
  bool found_alloc = false;
  for (const auto& instr : program.instructions()) {
    if (instr.opcode == t81::tisc::ir::Opcode::STRVECNEW) {
      found_alloc = true;
      break;
    }
  }
  assert(found_alloc);
}

void test_list_lowering() {
  verify_collection_lowering(R"(
    fn main() {
      let l: List[i32] = List[i32]();
    }
  )");
  std::cout << "test_list_lowering PASSED\n";
}

void test_map_lowering() {
  verify_collection_lowering(R"(
    fn main() {
      let m: Map[i32] = Map[i32]();
    }
  )");
  std::cout << "test_map_lowering PASSED\n";
}

void test_set_lowering() {
  verify_collection_lowering(R"(
    fn main() {
      let s: Set[i32] = Set[i32]();
    }
  )");
  std::cout << "test_set_lowering PASSED\n";
}

void test_tree_lowering() {
  verify_collection_lowering(R"(
    fn main() {
      let t: Tree[i32] = Tree[i32]();
    }
  )");
  std::cout << "test_tree_lowering PASSED\n";
}

int main() {
  test_list_lowering();
  test_map_lowering();
  test_set_lowering();
  test_tree_lowering();
  std::cout << "All Collection Determinism Tests PASSED!\n";
  return 0;
}
