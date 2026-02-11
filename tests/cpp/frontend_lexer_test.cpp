#include "t81/frontend/lexer.hpp"
#include <vector>
#include <iostream>

using namespace t81::frontend;

struct ExpectedToken {
    [[maybe_unused]] TokenType type;
    const char* lexeme;
    [[maybe_unused]] int line;
    [[maybe_unused]] int column;
};

bool test_sequence(const char* source, const std::vector<ExpectedToken>& expected_tokens) {
    auto expect = [](bool cond, const char* msg) -> bool {
        if (!cond) {
            std::cerr << "frontend_lexer_test failure: " << msg << "\n";
            return false;
        }
        return true;
    };

    Lexer lexer(source);
    [[maybe_unused]] std::vector<Token> tokens= lexer.all_tokens();
    if (tokens.size() != expected_tokens.size() + 1) {
        std::cerr << "frontend_lexer_test token count mismatch: expected "
                  << (expected_tokens.size() + 1) << " got " << tokens.size() << "\n";
        for (size_t i = 0; i < tokens.size(); ++i) {
            const auto& tok = tokens[i];
            std::cerr << "  [" << i << "] type=" << static_cast<int>(tok.type)
                      << " lexeme='" << tok.lexeme
                      << "' line=" << tok.line
                      << " col=" << tok.column << "\n";
        }
        return false;
    }

    for (size_t i = 0; i < expected_tokens.size(); ++i) {
        const auto& actual = tokens[i];
        const auto& expected = expected_tokens[i];
        if (actual.type != expected.type || actual.lexeme != expected.lexeme || actual.line != expected.line || actual.column != expected.column) {
            std::cerr << "Test failed at token index " << i << std::endl;
            std::cerr << "  Expected: Type=" << static_cast<int>(expected.type) << ", Lexeme='" << expected.lexeme << "', Line=" << expected.line << ", Col=" << expected.column << std::endl;
            std::cerr << "  Actual:   Type=" << static_cast<int>(actual.type) << ", Lexeme='" << actual.lexeme << "', Line=" << actual.line << ", Col=" << actual.column << std::endl;
        }
        if (!expect(actual.type == expected.type, "token type mismatch")) return false;
        if (!expect(actual.lexeme == expected.lexeme, "token lexeme mismatch")) return false;
        if (!expect(actual.line == expected.line, "token line mismatch")) return false;
        if (!expect(actual.column == expected.column, "token column mismatch")) return false;
    }
    if (!expect(tokens.back().type == TokenType::Eof, "missing EOF token")) return false;
    return true;
}

int main() {
    // Test a sequence of tokens with line and column numbers
    const char* source = R"(module my_mod;

fn main() -> i32 {
    let x = 1;
    return x;
}
)";
    std::vector<ExpectedToken> expected = {
        {TokenType::Module, "module", 1, 1},
        {TokenType::Identifier, "my_mod", 1, 8},
        {TokenType::Semicolon, ";", 1, 14},
        {TokenType::Fn, "fn", 3, 1},
        {TokenType::Identifier, "main", 3, 4},
        {TokenType::LParen, "(", 3, 8},
        {TokenType::RParen, ")", 3, 9},
        {TokenType::Arrow, "->", 3, 11},
        {TokenType::I32, "i32", 3, 14},
        {TokenType::LBrace, "{", 3, 18},
        {TokenType::Let, "let", 4, 5},
        {TokenType::Identifier, "x", 4, 9},
        {TokenType::Equal, "=", 4, 11},
        {TokenType::Integer, "1", 4, 13},
        {TokenType::Semicolon, ";", 4, 14},
        {TokenType::Return, "return", 5, 5},
        {TokenType::Identifier, "x", 5, 12},
        {TokenType::Semicolon, ";", 5, 13},
        {TokenType::RBrace, "}", 6, 1},
    };
    if (!test_sequence(source, expected)) return 1;

    std::cout << "All lexer tests passed!" << std::endl;

    const char* base81_source = "let i = 12t81; let f = 1.20t81;";
    std::vector<ExpectedToken> base81_expected = {
        {TokenType::Let, "let", 1, 1},
        {TokenType::Identifier, "i", 1, 5},
        {TokenType::Equal, "=", 1, 7},
        {TokenType::Base81Integer, "12t81", 1, 9},
        {TokenType::Semicolon, ";", 1, 14},
        {TokenType::Let, "let", 1, 16},
        {TokenType::Identifier, "f", 1, 20},
        {TokenType::Equal, "=", 1, 22},
        {TokenType::Base81Float, "1.20t81", 1, 24},
        {TokenType::Semicolon, ";", 1, 31},
    };
    if (!test_sequence(base81_source, base81_expected)) return 1;

    std::cout << "Base-81 lexer tests passed!" << std::endl;

    return 0;
}
