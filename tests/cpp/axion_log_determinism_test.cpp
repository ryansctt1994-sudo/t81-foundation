#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include "t81/tisc/program.hpp"
#include "t81/tisc/opcodes.hpp"
#include "t81/vm/vm.hpp"

using namespace t81::tisc;
using namespace t81::vm;

static bool expect(bool cond, const char* msg) {
    if (!cond) {
        std::cerr << "axion_log_determinism_test failure: " << msg << std::endl;
        return false;
    }
    return true;
}

void test_bounds_fault_log() {
    std::cout << "Testing Axion log determinism for BoundsFault..." << std::endl;

    // Instruction that triggers a bounds fault (Load from invalid address)
    Insn bad_load{Opcode::Load, 1, 9999, 0};
    Insn halt{Opcode::Halt};

    Program program;
    program.insns = {bad_load, halt};

    auto vm = make_interpreter_vm();
    vm->load_program(program);
    auto result = vm->run_to_halt();
    (void)result;

    if (!expect(!result.has_value(), "bounds test unexpectedly succeeded")) throw std::runtime_error("bounds expectation failed");
    if (!expect(result.error() == Trap::BoundsFault, "bounds test trap type mismatch")) throw std::runtime_error("bounds trap mismatch");

    bool found = false;
    for (const auto& event : vm->state().axion_log) {
        if (event.verdict.reason.find("bounds fault segment=") != std::string::npos) {
            std::cout << "  Found log: " << event.verdict.reason << std::endl;
            if (!expect(event.verdict.reason.find("segment=") != std::string::npos, "bounds log missing segment")) throw std::runtime_error("bounds log missing segment");
            if (!expect(event.verdict.reason.find("addr=9999") != std::string::npos, "bounds log missing addr=9999")) throw std::runtime_error("bounds log missing addr");
            if (!expect(event.verdict.reason.find("action=memory load") != std::string::npos, "bounds log missing action=memory load")) throw std::runtime_error("bounds log missing action");
            found = true;
        }
    }
    if (!expect(found, "bounds fault event not found in axion log")) throw std::runtime_error("bounds event not found");
    (void)found;
    std::cout << "  BoundsFault log verified." << std::endl;
}

void test_tensor_fault_log() {
    std::cout << "Testing Axion log determinism for Tensor handle fault..." << std::endl;

    Insn corrupt_handle{Opcode::LoadImm, 1, 42};
    corrupt_handle.literal_kind = LiteralKind::TensorHandle;
    Insn vec_add{Opcode::TVecAdd, 2, 1, 1}; // R2 = R1 + R1 (R1=42)
    Insn halt{Opcode::Halt};

    Program program;
    program.insns = {corrupt_handle, vec_add, halt};

    auto vm = make_interpreter_vm();
    vm->load_program(program);
    auto result = vm->run_to_halt();
    (void)result;

    if (!expect(!result.has_value(), "tensor fault test unexpectedly succeeded")) throw std::runtime_error("tensor expectation failed");

    bool found = false;
    for (const auto& event : vm->state().axion_log) {
        if (event.verdict.reason.find("bounds fault segment=tensor") != std::string::npos) {
            std::cout << "  Found log: " << event.verdict.reason << std::endl;
            if (!expect(event.verdict.reason.find("addr=42") != std::string::npos, "tensor log missing addr=42")) throw std::runtime_error("tensor log missing addr");
            if (!expect(event.verdict.reason.find("action=tensor handle access") != std::string::npos, "tensor log missing action")) throw std::runtime_error("tensor log missing action");
            found = true;
        }
    }
    if (!expect(found, "tensor fault event not found in axion log")) throw std::runtime_error("tensor event not found");
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
