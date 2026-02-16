#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include "t81/frontend/ir_generator.hpp"

using namespace t81::frontend;

void check_no_error(const std::string& source, const std::string& test_name) {
    std::cout << "Running test: " << test_name << "..." << std::endl;
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();
    if (parser.had_error()) {
        std::cerr << "Parser error in " << test_name << std::endl;
        exit(1);
    }

    SemanticAnalyzer analyzer(statements);
    analyzer.analyze();
    if (analyzer.had_error()) {
        for (const auto& d : analyzer.diagnostics()) {
            std::cerr << d.message << std::endl;
        }
        std::cerr << "Semantic analysis error in " << test_name << std::endl;
        exit(1);
    }

    IRGenerator ir_gen;
    ir_gen.attach_semantic_analyzer(&analyzer);
    try {
        [[maybe_unused]] auto prog = ir_gen.generate(statements);
    } catch (const std::exception& e) {
        std::cerr << "IR generation error in " << test_name << ": " << e.what() << std::endl;
        exit(1);
    }
    std::cout << "Passed." << std::endl;
}

int main() {
    // Test 1: If Expression
    std::string test1 = R"(
        fn main() -> i32 {
            let x: i32 = if (1 < 2) { 10 } else { 20 };
            return x;
        }
    )";
    check_no_error(test1, "If Expression");

    // Test 2: Block Expression
    std::string test2 = R"(
        fn main() -> i32 {
            let y: i32 = {
                let a: i32 = 5;
                a + 2
            };
            return y;
        }
    )";
    check_no_error(test2, "Block Expression");

    // Test 3: Nested If Expression in Block
    std::string test3 = R"(
        fn main() -> i32 {
            let z: i32 = {
                if (1 > 0) {
                    100
                } else {
                    200
                }
            };
            return z;
        }
    )";
    check_no_error(test3, "Nested If Expression in Block");

    // Test 4: If Expression with Else If
    std::string test4 = R"(
        fn main() -> i32 {
            let w: i32 = if (1 == 0) {
                1
            } else if (1 == 1) {
                2
            } else {
                3
            };
            return w;
        }
    )";
    check_no_error(test4, "If Expression with Else If");

    return 0;
}
