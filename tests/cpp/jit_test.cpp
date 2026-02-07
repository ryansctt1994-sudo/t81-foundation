#include "t81/vm/jit.hpp"
#include "t81/vm/state.hpp"
#include "t81/tisc/program.hpp"
#include <cassert>
#include <iostream>

using namespace t81::vm;

int main() {
    std::cout << "Starting HanoiVM JIT Prototype test...\n";

    [[maybe_unused]] State state;
    state.registers.fill(0);
    state.register_tags.fill(ValueTag::Int);

    state.registers[1] = 10;
    state.registers[2] = 20;

    [[maybe_unused]] JitCompiler compiler;
    compiler.start_tracing(0);

    [[maybe_unused]] t81::tisc::Insn insn1;
    insn1.opcode = t81::tisc::Opcode::Add;
    insn1.a = 0; insn1.b = 1; insn1.c = 2;
    compiler.record_instruction(insn1);

    [[maybe_unused]] t81::tisc::Insn insn2;
    insn2.opcode = t81::tisc::Opcode::Mul;
    insn2.a = 3; insn2.b = 0; insn2.c = 1;
    compiler.record_instruction(insn2);

    [[maybe_unused]] auto trace= compiler.compile();
    assert(trace != nullptr);

    trace->execute(state);

    std::cout << "R0: " << state.registers[0] << " (Expected 30)\n";
    std::cout << "R3: " << state.registers[3] << " (Expected 300)\n";

    assert(state.registers[0] == 30);
    assert(state.registers[3] == 300);

    std::cout << "HanoiVM JIT Prototype test passed!\n";
    [[maybe_unused]] return 0;
}
