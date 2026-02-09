#include "t81/vm/vm.hpp"
#include "t81/axion/engine.hpp"
#include "t81/axion/policy_engine.hpp"
#include "t81/cog/tier4/tier4_loop.hpp"
#include <cassert>
#include <iostream>
#include <vector>

using namespace t81::vm;
using namespace t81::tisc;

void run_tier4_test() {
    std::cout << "Running Tier 4 E2E VM Test...\n";

    // Prepare policy with meta limits
    std::string policy_text = "(policy (tier 4) (max-reflections 10) (max-meta-writes 100))";
    auto engine = t81::axion::make_policy_engine(t81::axion::parse_policy(policy_text).value());
    auto vm = make_interpreter_vm(std::move(engine));

    // Prepare TISC program
    // R0: counter
    // R1: heap address for commands
    // R2: handle from MetaReflect
    // R3: MetaRefine result
    Program program;
    program.insns = {
        {Opcode::LoadImm, 0, 0},        // 0: R0 = 0
        {Opcode::MetaReflect, 2, 0, 0}, // 1: R2 = handle
        {Opcode::LoadImm, 1, 100},      // 2: R1 = 100 (heap start-ish, depends on VM layout)
        // Commands in memory (to be written by test setup for simplicity, or we could use LOADI/STORE)
        {Opcode::LoadImm, 3, 1},        // 3: R3 = 1 (count)
        {Opcode::MetaRefine, 3, 1, 3},  // 4: R3 = MetaRefine(addr=R1, count=R3)
        {Opcode::Halt}                  // 5
    };

    vm->load_program(program);
    const auto& state = vm->state();

    // Manually setup memory for MetaRefine commands at state.layout.heap.start
    std::size_t cmd_addr = state.layout.heap.start;
    vm->set_register(1, static_cast<int64_t>(cmd_addr)); // R1 = cmd_addr

    // Command 1: WriteReg R0 = 42
    // Memory layout: [op, target, value, tag]
    auto& mutable_state = const_cast<State&>(vm->state());
    mutable_state.memory[cmd_addr] = static_cast<int64_t>(RefinementCommand::Op::WriteReg);
    mutable_state.memory[cmd_addr + 1] = 0; // R0
    mutable_state.memory[cmd_addr + 2] = 42; // value
    mutable_state.memory[cmd_addr + 3] = static_cast<int64_t>(ValueTag::Int);

    // Step through the program
    auto res = vm->run_to_halt(100);
    assert(res.has_value());
    assert(state.registers[0] == 42);
    assert(state.registers[3] == 1); // Success
    assert(state.reflection_count == 1);

    std::cout << "Tier 4 E2E VM Test passed!\n";
}

void run_determinism_test() {
    std::cout << "Running Tier 4 Determinism Test...\n";

    auto run_once = []() {
        std::string policy_text = "(policy (tier 4) (max-reflections 10) (max-meta-writes 100))";
        auto engine = t81::axion::make_policy_engine(t81::axion::parse_policy(policy_text).value());
        auto vm = make_interpreter_vm(std::move(engine));
        Program program;
        program.insns = {
            {Opcode::MetaReflect, 0, 0, 0},
            {Opcode::Halt}
        };
        vm->load_program(program);
        vm->run_to_halt(10);
        return vm->state();
    };

    State s1 = run_once();
    State s2 = run_once();

    assert(s1.registers[0] == s2.registers[0]);
    assert(s1.axion_log.size() == s2.axion_log.size());
    for (size_t i = 0; i < s1.axion_log.size(); ++i) {
        assert(s1.axion_log[i].verdict.reason == s2.axion_log[i].verdict.reason);
    }

    std::cout << "Tier 4 Determinism Test passed!\n";
}

void run_denial_test() {
    std::cout << "Running Tier 4 Denial Test...\n";

    // Policy with ZERO reflection budget
    std::string policy_text = "(policy (tier 4) (max-reflections 0))";
    auto engine = t81::axion::make_policy_engine(t81::axion::parse_policy(policy_text).value());
    auto vm = make_interpreter_vm(std::move(engine));

    Program program;
    program.insns = {
        {Opcode::MetaReflect, 0, 0, 0},
        {Opcode::Halt}
    };

    vm->load_program(program);
    auto res = vm->run_to_halt(10);

    assert(!res.has_value());
    assert(res.error() == Trap::SecurityFault);

    std::cout << "Tier 4 Denial Test passed!\n";
}

int main() {
    run_tier4_test();
    run_determinism_test();
    run_denial_test();
    return 0;
}
