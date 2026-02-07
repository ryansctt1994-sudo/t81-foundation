#include "t81/frontend/ir_generator.hpp"
#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/tisc/binary_emitter.hpp"
#include "t81/vm/vm.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void test_let_statement_e2e() {
    [[maybe_unused]] std::string source= "fn main() -> T81Int { let x: T81Int = 42; return x; }";
    t81::frontend::Lexer lexer(source);
    t81::frontend::Parser parser(lexer);
    [[maybe_unused]] auto stmts= parser.parse();

    assert(!parser.had_error() && "Parsing failed");

    [[maybe_unused]] t81::frontend::IRGenerator generator;
    [[maybe_unused]] auto ir_program= generator.generate(stmts);

    [[maybe_unused]] t81::tisc::BinaryEmitter emitter;
    [[maybe_unused]] auto program= emitter.emit(ir_program);

    [[maybe_unused]] auto vm= t81::vm::make_interpreter_vm();
    vm->load_program(program);
    vm->run_to_halt();

    // Per TISC calling convention, the return value is in R0.
    assert(vm->state().registers[0] == 42 && "VM register R0 has incorrect value");

    std::cout << "E2ETest test_let_statement_e2e passed!" << std::endl;
}

int main() {
    test_let_statement_e2e();
    return 0;
}
