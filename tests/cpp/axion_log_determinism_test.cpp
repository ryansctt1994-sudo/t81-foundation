#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include "t81/tisc/program.hpp"
#include "t81/tisc/opcodes.hpp"
#include "t81/vm/vm.hpp"

using namespace t81::tisc;
using namespace t81::vm;

void test_bounds_fault_log() {
    std::cout << "Testing Axion log determinism for BoundsFault..." << std::endl;

    // Instruction that triggers a bounds fault (Load from invalid address)
    Insn bad_load{Opcode::Load, 0, 9999, 0};
    Insn halt{Opcode::Halt};

    Program program;
    program.insns = {bad_load, halt};

    auto vm = make_interpreter_vm();
    vm->load_program(program);
    auto result = vm->run_to_halt();
    (void)result;

    assert(!result.has_value());
    assert(result.error() == Trap::BoundsFault);

    bool found = false;
    for (const auto& event : vm->state().axion_log) {
        if (event.verdict.reason.find("bounds fault segment=") != std::string::npos) {
            std::cout << "  Found log: " << event.verdict.reason << std::endl;
            assert(event.verdict.reason.find("segment=") != std::string::npos);
            assert(event.verdict.reason.find("addr=9999") != std::string::npos);
            assert(event.verdict.reason.find("action=memory load") != std::npos);
            found = true;
        }
    }
    assert(found);
    (void)found;
    std::cout << "  BoundsFault log verified." << std::endl;
}

void test_tensor_fault_log() {
    std::cout << "Testing Axion log determinism for Tensor handle fault..." << std::endl;

    Insn corrupt_handle{Opcode::LoadImm, 1, 42};
    corrupt_handle.literal_kind = LiteralKind::TensorHandle;
    Insn vec_add{Opcode::TVecAdd, 0, 1, 1}; // R0 = R1 + R1 (R1=42)
    Insn halt{Opcode::Halt};

    Program program;
    program.insns = {corrupt_handle, vec_add, halt};

    auto vm = make_interpreter_vm();
    vm->load_program(program);
    auto result = vm->run_to_halt();
    (void)result;

    assert(!result.has_value());

    bool found = false;
    for (const auto& event : vm->state().axion_log) {
        if (event.verdict.reason.find("bounds fault segment=tensor") != std::string::npos) {
            std::cout << "  Found log: " << event.verdict.reason << std::endl;
            assert(event.verdict.reason.find("addr=42") != std::string::npos);
            assert(event.verdict.reason.find("action=tensor handle access") != std::string::npos);
            found = true;
        }
    }
    assert(found);
    (void)found;
    std::cout << "  Tensor fault log verified." << std::endl;
}

int main() {
    try {
        test_bounds_fault_log();
        test_tensor_fault_log();
        std::cout << "All Axion log determinism tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
