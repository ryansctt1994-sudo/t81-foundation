#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>

using namespace t81::frontend;

// List of keywords and operators to generate valid lexemes
const std::vector<std::string> KEYWORDS = {
    "module", "type", "const", "export", "fn", "let", "var",
    "record", "enum",
    "if", "else", "for", "in", "while", "loop", "reflect", "break", "continue", "return", "match",
    "true", "false",
    "void", "bool", "i32", "i16", "i8", "i2",
    "BigInt", "Float", "Fraction", // Correct names for types? Based on usage, T81BigInt usually maps to BigInt keyword or similar?
    // Wait, let's check parser.cpp/lexer.cpp for keyword strings.
    // Assuming standard mapping for now based on enum names.
    "String", "Vector", "Matrix", "Tensor", "Graph"
};

const std::vector<std::string> OPERATORS = {
    "+", "-", "*", "/", "%",
    "=", "==", "!", "!=",
    "<", "<=", ">", ">=",
    "&", "&&", "|", "||", "^",
    "?",
    "(", ")", "{", "}", "[", "]",
    ",", ":", ";",
    "->", "=>", "..", ".", "@"
};

// Helper to generate random identifiers
std::string generate_identifier(std::mt19937& gen) {
    static const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    std::uniform_int_distribution<size_t> len_dist(1, 10);
    std::uniform_int_distribution<size_t> char_dist(0, chars.size() - 1);

    size_t length = len_dist(gen);
    std::string id;
    id.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        id += chars[char_dist(gen)];
    }
    return id;
}

// Helper to generate random integer literals
std::string generate_integer(std::mt19937& gen) {
    std::uniform_int_distribution<int> dist(0, 1000);
    return std::to_string(dist(gen));
}

// Helper to generate random string literals
std::string generate_string_literal(std::mt19937& gen) {
    static const std::string chars = "abcdefghijklmnopqrstuvwxyz 0123456789";
    std::uniform_int_distribution<size_t> len_dist(0, 20);
    std::uniform_int_distribution<size_t> char_dist(0, chars.size() - 1);

    size_t length = len_dist(gen);
    std::string s = "\"";
    for (size_t i = 0; i < length; ++i) {
        s += chars[char_dist(gen)];
    }
    s += "\"";
    return s;
}

// Helper to generate random lexemes
std::string generate_lexeme(std::mt19937& gen) {
    std::uniform_int_distribution<int> type_dist(0, 4);

    switch (type_dist(gen)) {
        case 0: // Keyword
            {
                std::uniform_int_distribution<size_t> idx_dist(0, KEYWORDS.size() - 1);
                return KEYWORDS[idx_dist(gen)];
            }
        case 1: // Operator/Punctuation
            {
                std::uniform_int_distribution<size_t> idx_dist(0, OPERATORS.size() - 1);
                return OPERATORS[idx_dist(gen)];
            }
        case 2: // Identifier
            return generate_identifier(gen);
        case 3: // Integer
            return generate_integer(gen);
        case 4: // String
            return generate_string_literal(gen);
        default:
            return " ";
    }
}

// Mode 1: Pure random stream of valid tokens
std::string generate_random_token_stream(std::mt19937& gen, size_t length) {
    std::string source;
    for (size_t i = 0; i < length; ++i) {
        source += generate_lexeme(gen);
        source += " "; // Space to separate tokens
    }
    return source;
}

// Mode 2: Structured fuzzing (balanced braces)
// This is a simple recursive generator to try and create nested structures
void generate_structure(std::mt19937& gen, std::string& source, int depth) {
    if (depth > 5) {
        source += generate_lexeme(gen) + " ";
        return;
    }

    std::uniform_int_distribution<int> choice(0, 5);
    int c = choice(gen);

    if (c == 0) {
        // Block: { ... }
        source += "{ ";
        int items = std::uniform_int_distribution<int>(0, 5)(gen);
        for(int i=0; i<items; ++i) generate_structure(gen, source, depth + 1);
        source += "} ";
    } else if (c == 1) {
        // Paren: ( ... )
        source += "( ";
        int items = std::uniform_int_distribution<int>(0, 3)(gen);
        for(int i=0; i<items; ++i) generate_structure(gen, source, depth + 1);
        source += ") ";
    } else if (c == 2) {
        // Function-ish: fn id() { ... }
        source += "fn " + generate_identifier(gen) + "() { ";
        generate_structure(gen, source, depth + 1);
        source += "} ";
    } else if (c == 3) {
        // Let: let id = ...;
        source += "let " + generate_identifier(gen) + " = " + generate_integer(gen) + "; ";
    } else {
        // Random sequence
        source += generate_lexeme(gen) + " ";
    }
}

void fuzz_iteration(std::mt19937& gen, int iteration) {
    std::string input;

    // Mix strategies
    if (iteration % 2 == 0) {
        size_t length = std::uniform_int_distribution<size_t>(10, 200)(gen);
        input = generate_random_token_stream(gen, length);
    } else {
        generate_structure(gen, input, 0);
    }

    // Uncomment to debug specific crashes
    // std::cout << "Input length: " << input.length() << "\n";

    try {
        Lexer lexer(input);
        Parser parser(lexer, "fuzz_test");
        auto stmts = parser.parse();

        if (!parser.had_error()) {
            SemanticAnalyzer analyzer(stmts);
            analyzer.analyze();
        }
    } catch (const std::exception& e) {
        std::cerr << "Caught exception on iteration " << iteration << ": " << e.what() << "\n";
        // We don't want to abort on C++ exceptions if they are expected (e.g. parser errors thrown as exceptions?),
        // but normally the parser uses return values/error flags.
        // If it throws, it might be a bug or intended.
        // For now, let's catch standard exceptions but abort on unknown ones or segfaults (implicit).
    } catch (...) {
        std::cerr << "Crash (unknown exception) on iteration " << iteration << "\n";
        std::cerr << "Input was:\n" << input << "\n";
        std::abort();
    }
}

int main() {
    std::random_device rd;
    unsigned int seed = rd();
    std::cout << "Seed: " << seed << "\n"; // Print seed for reproducibility
    std::mt19937 gen(seed);

    int iterations = 2000;
    std::cout << "Running " << iterations << " fuzzing iterations...\n";

    for (int i = 0; i < iterations; ++i) {
        if (i % 100 == 0) std::cout << "Iteration " << i << "...\n";
        fuzz_iteration(gen, i);
    }
    std::cout << "\nFuzzing completed successfully!\n";
    return 0;
}
