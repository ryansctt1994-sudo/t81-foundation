#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"

#include <cassert>
#include <iostream>
#include <string>

using namespace t81::frontend;

void expect_semantic_success(const std::string& source, const char* label) {
    Lexer lexer(source);
    Parser parser(lexer);
    [[maybe_unused]] auto stmts = parser.parse();
    if (parser.had_error()) {
        std::cerr << "[" << label << "] parser reported errors\n";
    }
    assert(!parser.had_error());

    SemanticAnalyzer analyzer(stmts);
    analyzer.analyze();
    if (analyzer.had_error()) {
        std::cerr << "[" << label << "] semantic analysis failed (unexpected)\n";
        for (const auto& d : analyzer.diagnostics()) {
            std::cerr << "  Line " << d.line << ": " << d.message << "\n";
        }
    }
    assert(!analyzer.had_error());
}

int main() {
    // 1D Vector indexing - should pass already
    const std::string vector_indexing = R"(
        fn main() -> i32 {
            let v: Vector[i32, 2] = [1, 2];
            let x: i32 = v[0];
            return x;
        }
    )";
    expect_semantic_success(vector_indexing, "vector_indexing");

    // 2D Vector indexing (Vector of Vectors)
    const std::string nested_vector_indexing = R"(
        fn main() -> i32 {
            let v: Vector[Vector[i32, 2], 2] = [[1, 2], [3, 4]];
            let x: i32 = v[0][0];
            return x;
        }
    )";
    expect_semantic_success(nested_vector_indexing, "nested_vector_indexing");

    // 1D Tensor indexing - Tensor[T, N] behaves like Vector[T, N]
    const std::string tensor_1d_indexing = R"(
        fn main(t: Tensor[i32, 2]) -> i32 {
            let x: i32 = t[0];
            return x;
        }
    )";
    expect_semantic_success(tensor_1d_indexing, "tensor_1d_indexing");

    // 2D Tensor indexing - Tensor[T, N, M]
    // t[0] should return Tensor[T, M] (slice)
    // t[0][0] should return T
    const std::string tensor_2d_indexing = R"(
        fn main(t: Tensor[i32, 2, 2]) -> i32 {
            let row: Tensor[i32, 2] = t[0];
            let x: i32 = t[0][0];
            return x;
        }
    )";
    expect_semantic_success(tensor_2d_indexing, "tensor_2d_indexing");

    // Assignment to index
    const std::string vector_assignment = R"(
        fn main() -> i32 {
            var v: Vector[i32, 2] = [1, 2];
            v[0] = 5;
            return v[0];
        }
    )";
    expect_semantic_success(vector_assignment, "vector_assignment");

    std::cout << "All indexing tests passed!\n";
    return 0;
}
