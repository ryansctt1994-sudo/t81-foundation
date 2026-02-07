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

// Helper to compile and run a T81Lang source string and return the final value of register r0.
int64_t run_e2e_test(const std::string& source) {
    frontend::Lexer lexer(source);
    frontend::Parser parser(lexer);
    [[maybe_unused]] auto stmts= parser.parse();
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
    [[maybe_unused]] tisc::ir::IntermediateProgram ir= ir_gen.generate(stmts);

    [[maybe_unused]] tisc::BinaryEmitter emitter;
    [[maybe_unused]] tisc::Program program= emitter.emit(ir);

    [[maybe_unused]] auto vm= vm::make_interpreter_vm();
    vm->load_program(program);
    vm->run_to_halt();

    return vm->state().registers[0];
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
    [[maybe_unused]] int64_t result= run_e2e_test(source);
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
    [[maybe_unused]] int64_t result= run_e2e_test(source);
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
    [[maybe_unused]] int64_t result= run_e2e_test(source);
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
    [[maybe_unused]] int64_t result= run_e2e_test(source);
    if (result != 42) {
        std::cerr << "test_custom_enum_match failed: expected 42, got " << result << std::endl;
        throw std::runtime_error("test_custom_enum_match failed");
    }
}

int main() {
    test_while_break();
    test_nested_loop_continue();
    test_match_guards();
    test_custom_enum_match();
    std::cout << "All advanced E2E tests passed!" << std::endl;
    return 0;
}
