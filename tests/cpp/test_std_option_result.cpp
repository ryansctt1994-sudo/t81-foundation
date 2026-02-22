#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include "t81/frontend/ir_generator.hpp"
#include <iostream>
#include <cassert>

using namespace t81::frontend;

void check(const std::string& source, const char* name) {
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();
    if (parser.had_error()) {
        std::cerr << name << ": Parser error\n";
        exit(1);
    }

    SemanticAnalyzer analyzer(stmts);
    analyzer.analyze();
    if (analyzer.had_error()) {
        std::cerr << name << ": Semantic error\n";
        for (auto& d : analyzer.diagnostics()) {
            std::cerr << d.message << "\n";
        }
        exit(1);
    }

    IRGenerator generator;
    generator.attach_semantic_analyzer(&analyzer);
    auto prog = generator.generate(stmts);
    if (prog.instructions().empty()) {
        std::cerr << name << ": No instructions generated\n";
        exit(1);
    }
    std::cout << name << ": OK\n";
}

int main() {
    check(R"(
        fn main() -> bool {
            let x: Option[i32] = Some(10);
            return std.option.is_some(x);
        }
    )", "option_is_some");

    check(R"(
        fn main() -> bool {
            let x: Option[i32] = None;
            return std.option.is_none(x);
        }
    )", "option_is_none");

    check(R"(
        fn main() -> i32 {
            let x: Option[i32] = Some(10);
            return std.option.unwrap(x);
        }
    )", "option_unwrap");

    check(R"(
        fn main() -> bool {
            let x: Result[i32, i32] = Ok(10);
            return std.result.is_ok(x);
        }
    )", "result_is_ok");

    check(R"(
        fn main() -> bool {
            let x: Result[i32, i32] = Err(10);
            return std.result.is_err(x);
        }
    )", "result_is_err");

    check(R"(
        fn main() -> i32 {
            let x: Result[i32, i32] = Ok(10);
            return std.result.unwrap(x);
        }
    )", "result_unwrap");

    check(R"(
        fn main() -> i32 {
            let x: Result[i32, i32] = Err(10);
            return std.result.unwrap_err(x);
        }
    )", "result_unwrap_err");

    return 0;
}
