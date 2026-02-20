#include <cassert>
#include <iostream>
#include <vector>
#include "t81/tisc/opcodes.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

using namespace t81::tisc;
using namespace t81::vm;

void test_symbolic_rewrite() {
  Program prog;
  prog.symbol_pool = {"nodeA", "nodeB"};

  std::vector<Insn> insns;

  // 0: LoadImm R2, :nodeA
  Insn i0;
  i0.opcode = Opcode::LoadImm;
  i0.a = 2;  // R2
  i0.b = 1;  // "nodeA" handle
  i0.literal_kind = LiteralKind::SymbolHandle;
  insns.push_back(i0);

  // 1: SymLoad R1, R2
  Insn i1;
  i1.opcode = Opcode::SymLoad;
  i1.a = 1;  // R1
  i1.b = 2;  // R2
  insns.push_back(i1);

  // 2: LoadImm R3, :nodeA (Match)
  Insn i2;
  i2.opcode = Opcode::LoadImm;
  i2.a = 3;
  i2.b = 1;  // "nodeA"
  i2.literal_kind = LiteralKind::SymbolHandle;
  insns.push_back(i2);

  // 3: LoadImm R4, :nodeB (Replace)
  Insn i3;
  i3.opcode = Opcode::LoadImm;
  i3.a = 4;
  i3.b = 2;  // "nodeB"
  i3.literal_kind = LiteralKind::SymbolHandle;
  insns.push_back(i3);

  // 4: SymRewrite R1, R3, R4
  Insn i4;
  i4.opcode = Opcode::SymRewrite;
  i4.a = 1;  // R1 (Graph)
  i4.b = 3;  // R3 (Match)
  i4.c = 4;  // R4 (Replace)
  insns.push_back(i4);

  // 5: SymCanon R1
  Insn i5;
  i5.opcode = Opcode::SymCanon;
  i5.a = 1;
  insns.push_back(i5);

  // 6: Halt
  Insn i6;
  i6.opcode = Opcode::Halt;
  insns.push_back(i6);

  prog.insns = insns;

  auto vm = make_interpreter_vm();
  vm->load_program(prog);
  auto res = vm->run_to_halt();

  if (!res.has_value()) {
    std::cerr << "VM Error: " << (int)res.error() << std::endl;
    exit(1);
  }

  const auto& state = vm->state();
  if (state.registers[1] <= 0) {
    std::cerr << "Invalid handle in R1" << std::endl;
    exit(1);
  }
  if (state.register_tags[1] != ValueTag::SymbolicGraphHandle) {
    std::cerr << "Invalid tag in R1: " << (int)state.register_tags[1] << std::endl;
    exit(1);
  }

  // Verify graph content
  const auto& graph = state.symbolic_graphs[state.registers[1] - 1];
  if (graph.nodes.size() != 1) {
    std::cerr << "Graph node count mismatch: " << graph.nodes.size() << std::endl;
    exit(1);
  }

  auto idB = t81::T81Symbol::intern("nodeB");
  // Debug info
  // std::cout << "Node 0 ID: " << graph.nodes[0].id.to_string() << std::endl;

  if (graph.nodes[0].id != idB) {
    std::cerr << "Rewrite failed: node ID mismatch." << std::endl;
    exit(1);
  }

  std::cout << "Symbolic rewrite test passed!" << std::endl;
}

int main() {
  test_symbolic_rewrite();
  return 0;
}
