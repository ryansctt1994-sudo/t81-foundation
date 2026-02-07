#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace t81::frontend;

std::string generate_random_string(std::mt19937& gen, size_t length) {
    [[maybe_unused]] std::string charset= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-*/%=<>()[]{}; \n\t@\"\\";
    std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);
    [[maybe_unused]] std::string result;
    for (size_t i = 0; i < length; ++i) {
        result += charset[dist(gen)];
    }
    return result;
}

void fuzz_iteration(std::mt19937& gen, int iteration) {
    [[maybe_unused]] size_t length= std::uniform_int_distribution<size_t>(1, 1000)(gen);
    [[maybe_unused]] std::string input= generate_random_string(gen, length);

    try {
        Lexer lexer(input);
        // Exhaust tokens
        [[maybe_unused]] auto tokens= lexer.all_tokens();

        Parser parser(lexer);
        [[maybe_unused]] auto stmts= parser.parse();

        if (!parser.had_error()) {
            SemanticAnalyzer analyzer(stmts);
            analyzer.analyze();
        }
    } catch (...) {
        std::cerr << "Crash on iteration " << iteration << " with input:\n" << input << "\n";
        std::abort();
    }
}

int main() {
    [[maybe_unused]] std::random_device rd;
    std::mt19937 gen(rd());

    [[maybe_unused]] int iterations= 1000;
    std::cout << "Running " << iterations << " fuzzing iterations...\n";
    for (int i = 0; i < iterations; ++i) {
        if (i % 100 == 0) {
            std::cout << "Iteration " << i << "...\n";
        }
        fuzz_iteration(gen, i);
    }
    std::cout << "Fuzzing completed successfully!\n";
    return 0;
}
