#include <cassert>
#include <iostream>
#include <vector>
#include "t81/isa/opcodes.hpp"
#include "t81/isa/program.hpp"
#include "t81/vm/vm.hpp"

void test_gossip() {
  std::cout << "Testing Gossip..." << std::endl;
  auto vm = t81::vm::make_interpreter_vm(nullptr);

  t81::tisc::Program prog;
  t81::tisc::Insn insn;
  insn.opcode = t81::tisc::Opcode::Gossip;
  insn.b = 1;  // R1
  prog.insns.push_back(insn);

  // Halt
  t81::tisc::Insn halt;
  halt.opcode = t81::tisc::Opcode::Halt;
  prog.insns.push_back(halt);

  vm->load_program(prog);
  vm->set_register(1, 42, t81::vm::ValueTag::Int);

  auto res = vm->run_to_halt(10);
  if (!res) {
    std::cerr << "VM Error: " << (int)res.error() << std::endl;
    exit(1);
  }

  [[maybe_unused]] const auto& state = vm->state();
  assert(!state.tier4_state.outbox.empty());
  assert(state.tier4_state.outbox.back().payload == 42);
  std::cout << "Gossip OK" << std::endl;
}

void test_merge() {
  std::cout << "Testing Merge..." << std::endl;
  auto vm = t81::vm::make_interpreter_vm(nullptr);

  t81::tisc::Program prog;

  // Merge R1
  t81::tisc::Insn merge_insn;
  merge_insn.opcode = t81::tisc::Opcode::Merge;
  merge_insn.a = 1;  // R1
  prog.insns.push_back(merge_insn);

  // Halt
  t81::tisc::Insn halt;
  halt.opcode = t81::tisc::Opcode::Halt;
  prog.insns.push_back(halt);

  vm->load_program(prog);

  // Inject message
  auto& mutable_state = const_cast<t81::vm::State&>(vm->state());
  t81::cog::v4::NetworkMessage msg;
  msg.payload = 123;
  msg.tag = static_cast<int32_t>(t81::vm::ValueTag::Int);
  msg.tick = 100;
  mutable_state.tier4_state.inbox.push_back(msg);

  auto res = vm->run_to_halt(10);
  if (!res) {
    std::cerr << "VM Error: " << (int)res.error() << std::endl;
    exit(1);
  }

  const auto& state = vm->state();
  // R1 should be an OptionHandle
  assert(state.contexts[0].register_tags[1] == t81::vm::ValueTag::OptionHandle);

  // Verify content of option
  int64_t handle = state.contexts[0].registers[1];
  assert(handle > 0);
  [[maybe_unused]] const auto& opt = state.options[handle - 1];
  assert(opt.has_value);
  assert(opt.payload == 123);
  assert(opt.payload_tag == t81::vm::ValueTag::Int);

  // Also check tick sync
  assert(state.tier4_state.vector.global_tick >= 100);

  std::cout << "Merge OK" << std::endl;
}

void test_sync() {
  std::cout << "Testing Sync..." << std::endl;
  auto vm = t81::vm::make_interpreter_vm(nullptr);

  t81::tisc::Program prog;

  // LoadImm R1, 500
  t81::tisc::Insn load;
  load.opcode = t81::tisc::Opcode::LoadImm;
  load.a = 1;
  load.b = 500;
  load.literal_kind = t81::tisc::LiteralKind::Int;
  prog.insns.push_back(load);

  // TickSync R1
  t81::tisc::Insn sync;
  sync.opcode = t81::tisc::Opcode::TickSync;
  sync.a = 1;
  prog.insns.push_back(sync);

  // Halt
  t81::tisc::Insn halt;
  halt.opcode = t81::tisc::Opcode::Halt;
  prog.insns.push_back(halt);

  vm->load_program(prog);

  auto res = vm->run_to_halt(10);
  if (!res) {
    std::cerr << "VM Error: " << (int)res.error() << std::endl;
    exit(1);
  }

  [[maybe_unused]] const auto& state = vm->state();
  assert(state.tier4_state.vector.global_tick >= 500);

  std::cout << "Sync OK" << std::endl;
}

void test_merge_prefers_newest_tick() {
  std::cout << "Testing Merge newest-tick selection..." << std::endl;
  auto vm = t81::vm::make_interpreter_vm(nullptr);

  t81::tisc::Program prog;
  t81::tisc::Insn merge_insn;
  merge_insn.opcode = t81::tisc::Opcode::Merge;
  merge_insn.a = 1;
  prog.insns.push_back(merge_insn);
  prog.insns.push_back({t81::tisc::Opcode::Halt, 0, 0, 0});
  vm->load_program(prog);

  auto& mutable_state = const_cast<t81::vm::State&>(vm->state());
  mutable_state.tier4_state.inbox.push_back(
      {111, static_cast<int32_t>(t81::vm::ValueTag::Int), 10, "node-a"});
  mutable_state.tier4_state.inbox.push_back(
      {222, static_cast<int32_t>(t81::vm::ValueTag::Int), 20, "node-b"});

  auto res = vm->run_to_halt(10);
  if (!res) {
    std::cerr << "VM Error: " << (int)res.error() << std::endl;
    exit(1);
  }

  const auto& state = vm->state();
  int64_t handle = state.contexts[0].registers[1];
  assert(handle > 0);
  [[maybe_unused]] const auto& opt = state.options[handle - 1];
  assert(opt.has_value);
  assert(opt.payload == 222);
  std::cout << "Merge newest-tick OK" << std::endl;
}

void test_merge_deduplicates_identical_messages() {
  std::cout << "Testing Merge deduplication..." << std::endl;
  t81::cog::v4::NodeState node;
  node.inbox.push_back({7, static_cast<int32_t>(t81::vm::ValueTag::Int), 9, "peer"});
  node.inbox.push_back({7, static_cast<int32_t>(t81::vm::ValueTag::Int), 9, "peer"});

  auto first = node.merge();
  assert(first.has_value());
  auto second = node.merge();
  assert(!second.has_value());
  std::cout << "Merge deduplication OK" << std::endl;
}

int main() {
  test_gossip();
  test_merge();
  test_sync();
  test_merge_prefers_newest_tick();
  test_merge_deduplicates_identical_messages();
  return 0;
}
