#include <cassert>
#include <iostream>
#include <vector>
#include "t81/tisc/opcodes.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

// Test Tier 5 Opcodes: InfSeed, InfExpand, InfCollapse, InfConverge, InfSignature

int main() {
  using namespace t81::tisc;
  using namespace t81::vm;
  Program prog;

  // 1. Create Fraction 1/2
  // R1 = 1
  prog.insns.push_back({Opcode::LoadImm, 1, 1, 0, LiteralKind::Int});
  // R2 = Fraction(1)
  prog.insns.push_back({Opcode::I2Frac, 2, 1, 0});

  // R3 = 2
  prog.insns.push_back({Opcode::LoadImm, 3, 2, 0, LiteralKind::Int});
  // R4 = Fraction(2)
  prog.insns.push_back({Opcode::I2Frac, 4, 3, 0});

  // R5 = Fraction(1) / Fraction(2) = 1/2
  prog.insns.push_back({Opcode::FracDiv, 5, 2, 4});

  // 2. Convergent Series: a=1, r=1/2. Sum = 1 / (1 - 0.5) = 2.
  // InfSeed R10, R2 (start = 1)
  prog.insns.push_back({Opcode::InfSeed, 10, 2, 0});
  // InfExpand R10, R5 (ratio = 1/2)
  prog.insns.push_back({Opcode::InfExpand, 10, 5, 0});
  // InfCollapse R10
  prog.insns.push_back({Opcode::InfCollapse, 10, 0, 0});
  // InfConverge R11, R10
  prog.insns.push_back({Opcode::InfConverge, 11, 10, 0});
  // InfSignature R12, R10
  prog.insns.push_back({Opcode::InfSignature, 12, 10, 0});

  // 3. Divergent Series: a=1, r=2.
  // InfSeed R20, R2 (start = 1)
  prog.insns.push_back({Opcode::InfSeed, 20, 2, 0});
  // InfExpand R20, R4 (ratio = 2)
  prog.insns.push_back({Opcode::InfExpand, 20, 4, 0});
  // InfCollapse R20
  prog.insns.push_back({Opcode::InfCollapse, 20, 0, 0});
  // InfConverge R21, R20
  prog.insns.push_back({Opcode::InfConverge, 21, 20, 0});
  // InfSignature R22, R20
  prog.insns.push_back({Opcode::InfSignature, 22, 20, 0});

  // Halt
  prog.insns.push_back({Opcode::Halt, 0, 0, 0});

  auto vm = t81::vm::make_interpreter_vm(nullptr);
  vm->load_program(prog);

  auto result = vm->run_to_halt(100);
  if (!result) {
    std::cerr << "VM Error: " << static_cast<int>(result.error()) << std::endl;
    return 1;
  }

  const auto& state = vm->state();
  bool failed = false;

  // Check Convergent Series
  // R11 should be 1 (true)
  if (state.contexts[0].registers[11] != 1) {
    std::cerr << "Test Failed: R11 (Convergent check) should be 1, got " << state.contexts[0].registers[11]
              << "\n";
    failed = true;
  }

  // R12 should be symbol "GEOMETRIC_SUM_2.000000"
  if (state.contexts[0].register_tags[12] == ValueTag::SymbolHandle) {
    int64_t handle = state.contexts[0].registers[12];
    if (handle > 0 && (size_t)(handle - 1) < state.symbols.size()) {
      std::string sig = state.symbols[handle - 1];
      std::cout << "Convergent Signature: " << sig << "\n";
      if (sig.find("GEOMETRIC_SUM_2") == std::string::npos) {
        std::cerr << "Test Failed: Signature expected GEOMETRIC_SUM_2..., got " << sig << "\n";
        failed = true;
      }
    } else {
      std::cerr << "Test Failed: Invalid symbol handle for R12\n";
      failed = true;
    }
  } else {
    std::cerr << "Test Failed: R12 tag is not SymbolHandle\n";
    failed = true;
  }

  // Check Divergent Series
  // R21 should be 0 (false)
  if (state.contexts[0].registers[21] != 0) {
    std::cerr << "Test Failed: R21 (Divergent check) should be 0, got " << state.contexts[0].registers[21]
              << "\n";
    failed = true;
  }

  // R22 should be symbol "DIVERGENT" (from hash assignment in infinite.cpp which assigns
  // "DIVERGENT")
  if (state.contexts[0].register_tags[22] == ValueTag::SymbolHandle) {
    int64_t handle = state.contexts[0].registers[22];
    if (handle > 0 && (size_t)(handle - 1) < state.symbols.size()) {
      std::string sig = state.symbols[handle - 1];
      std::cout << "Divergent Signature: " << sig << "\n";
      if (sig != "DIVERGENT") {
        std::cerr << "Test Failed: Signature expected DIVERGENT, got " << sig << "\n";
        failed = true;
      }
    }
  }

  if (failed) {
    return 1;
  }

  std::cout << "Test Passed: Tier 5 opcodes executed correctly." << std::endl;
  return 0;
}
