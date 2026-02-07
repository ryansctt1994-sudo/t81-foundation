#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include "t81/frontend/ir_generator.hpp"
#include "t81/tisc/binary_emitter.hpp"
#include "t81/vm/vm.hpp"
#include "t81/vm/state.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace t81;

int64_t run_e2e_test(const std::string& source) {
    frontend::Lexer lexer(source);
    frontend::Parser parser(lexer);
    [[maybe_unused]] auto stmts= parser.parse();
    assert(!parser.had_error());

    frontend::SemanticAnalyzer analyzer(stmts);
    analyzer.analyze();
    assert(!analyzer.had_error());

    [[maybe_unused]] frontend::IRGenerator ir_gen;
    ir_gen.attach_semantic_analyzer(&analyzer);
    [[maybe_unused]] tisc::ir::IntermediateProgram ir= ir_gen.generate(stmts);

    [[maybe_unused]] tisc::BinaryEmitter emitter;
    [[maybe_unused]] tisc::Program program= emitter.emit(ir);

    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(program);
    vm->run_to_halt();

    return vm->state().registers[0];
}

void test_option() {
    const std::string source = R"(
        fn get_some() -> Option[i32] { return Some(123); }
        fn get_none() -> Option[i32] { return None; }

        fn main() -> i32 {
            let a: i32 = match (get_some()) {
                Some(v) => v,
                None => 0
            };
            let b: i32 = match (get_none()) {
                Some(v) => v,
                None => 42
            };
            return a + b; // 123 + 42 = 165
        }
    )";
    assert(run_e2e_test(source) == 165);
}

void test_result() {
    const std::string source = R"(
        fn get_ok() -> Result[i32, i32] { return Ok(7); }
        fn get_err() -> Result[i32, i32] { return Err(99); }

        fn main() -> i32 {
            let a: i32 = match (get_ok()) {
                Ok(v) => v,
                Err(e) => 0
            };
            let b: i32 = match (get_err()) {
                Ok(v) => v,
                Err(e) => e
            };
            return a + b; // 7 + 99 = 106
        }
    )";
    assert(run_e2e_test(source) == 106);
}

int main() {
    test_option();
    test_result();
    std::cout << "E2E option/result test passed!" << std::endl;
    return 0;
}
