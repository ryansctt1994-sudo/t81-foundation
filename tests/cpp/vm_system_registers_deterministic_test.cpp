#include "t81/isa/opcodes.hpp"
#include "t81/isa/program.hpp"
#include "t81/vm/vm.hpp"

#include "test_runtime_check.hpp"

namespace {

t81::tisc::Program make_program_a() {
  t81::tisc::Program p;
  p.insns.push_back({t81::tisc::Opcode::Nop, 0, 0, 0});
  p.insns.push_back({t81::tisc::Opcode::Halt, 0, 0, 0});
  return p;
}

t81::tisc::Program make_program_b() {
  t81::tisc::Program p;
  p.insns.push_back({t81::tisc::Opcode::LoadImm, 1, 7, 0, t81::tisc::LiteralKind::Int});
  p.insns.push_back({t81::tisc::Opcode::Halt, 0, 0, 0});
  return p;
}

void run_lineage_signature_test() {
  auto vm_a1 = t81::vm::make_interpreter_vm();
  auto vm_a2 = t81::vm::make_interpreter_vm();
  auto vm_b = t81::vm::make_interpreter_vm();

  auto a = make_program_a();
  auto b = make_program_b();

  vm_a1->load_program(a);
  vm_a2->load_program(a);
  vm_b->load_program(b);

  const auto& s_a1 = vm_a1->state().contexts[0];
  const auto& s_a2 = vm_a2->state().contexts[0];
  const auto& s_b = vm_b->state().contexts[0];

  T81_TEST_CHECK(s_a1.registers[76] == s_a2.registers[76]);
  T81_TEST_CHECK(s_a1.registers[76] != s_b.registers[76]);
}

void run_constitutional_mask_test() {
  t81::tisc::Program p;
  p.insns.push_back({t81::tisc::Opcode::Nop, 0, 0, 0});
  p.insns.push_back({t81::tisc::Opcode::Halt, 0, 0, 0});
  p.axion_policy_text = "(policy (max-instructions 5) (max-recursion 2) "
                        "(allowed-tensor-hashes [\"sha3:abc\"]))";

  auto vm = t81::vm::make_interpreter_vm();
  vm->load_program(p);
  auto result = vm->run_to_halt();
  T81_TEST_CHECK(result.has_value());

  const auto& ctx = vm->state().contexts[0];
  constexpr std::int64_t kPolicyLoaded = (1ll << 0);
  constexpr std::int64_t kMaxInstructions = (1ll << 1);
  constexpr std::int64_t kMaxRecursion = (1ll << 2);
  constexpr std::int64_t kAllowedTensorHashes = (1ll << 6);

  T81_TEST_CHECK((ctx.registers[78] & kPolicyLoaded) != 0);
  T81_TEST_CHECK((ctx.registers[78] & kMaxInstructions) != 0);
  T81_TEST_CHECK((ctx.registers[78] & kMaxRecursion) != 0);
  T81_TEST_CHECK((ctx.registers[78] & kAllowedTensorHashes) != 0);
}

}  // namespace

int main() {
  run_lineage_signature_test();
  run_constitutional_mask_test();
  return 0;
}
