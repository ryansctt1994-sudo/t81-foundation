#include "t81/vm/jit.hpp"
#include "t81/vm/state.hpp"
#include "t81/tisc/program.hpp"
#include <iostream>
#include <string>

using namespace t81::vm;

namespace {
bool expect(bool cond, const std::string& message) {
    if (!cond) {
        std::cerr << "jit_test failure: " << message << "\n";
        return false;
    }
    return true;
}
}

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
    insn1.a = 4; insn1.b = 1; insn1.c = 2; // R4 = R1 + R2 = 10 + 20 = 30
    compiler.record_instruction(insn1);

    [[maybe_unused]] t81::tisc::Insn insn2;
    insn2.opcode = t81::tisc::Opcode::Mul;
    insn2.a = 3; insn2.b = 4; insn2.c = 1; // R3 = R4 * R1 = 30 * 10 = 300
    compiler.record_instruction(insn2);

    [[maybe_unused]] auto trace = compiler.compile();
    if (!expect(trace != nullptr, "trace compilation returned null")) {
        return 1;
    }

    [[maybe_unused]] auto exec = trace->execute(state);

    std::cout << "R4: " << state.registers[4] << " (Expected 30)\n";
    std::cout << "R3: " << state.registers[3] << " (Expected 300)\n";

    if (!expect(state.registers[4] == 30, "R4 mismatch after Add")) {
        return 1;
    }
    if (!expect(state.registers[3] == 300, "R3 mismatch after Mul")) {
        return 1;
    }

    // Guard/deopt behavior: tensor trace op on non-tensor tags should return a
    // deterministic guard-deopt classification.
    JitCompiler deopt_compiler;
    deopt_compiler.start_tracing(0);
    t81::tisc::Insn tmatmul{};
    tmatmul.opcode = t81::tisc::Opcode::TMatMul;
    tmatmul.a = 4;
    tmatmul.b = 1;
    tmatmul.c = 2;
    deopt_compiler.record_instruction(tmatmul);
    auto deopt_trace = deopt_compiler.compile();
    if (!expect(deopt_trace != nullptr, "deopt trace compilation returned null")) {
        return 1;
    }
    [[maybe_unused]] const auto deopt_result = deopt_trace->execute(state);
    if (!expect(deopt_result.instructions_executed == 1, "guard deopt instruction count mismatch")) {
        return 1;
    }
    if (!expect(deopt_result.exit_kind == JitTrace::ExitKind::GuardDeopt, "guard deopt exit kind mismatch")) {
        return 1;
    }

    std::cout << "HanoiVM JIT Prototype test passed!\n";
    return 0;
}
