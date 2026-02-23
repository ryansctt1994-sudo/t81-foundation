#include <iostream>
#include <sstream>
#include "t81/axion/policy.hpp"
#include "t81/axion/policy_engine.hpp"

int main() {
  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "axion_policy_bytecode_test failure: " << msg << "\n";
      return false;
    }
    return true;
  };

  using namespace t81::axion;

  // 1. Create a policy with S-expression DSL
  [[maybe_unused]] std::string dsl = "(policy (tier 3) (max-instructions 100))";
  [[maybe_unused]] auto policy_res = parse_policy(dsl);
  if (!expect(policy_res.has_value(), "parse_policy failed")) return 1;
  [[maybe_unused]] Policy policy = std::move(policy_res.value());

  // 2. Compile to bytecode
  policy.compile_to_bytecode();
  if (!expect(!policy.bytecode.empty(), "bytecode is empty")) return 1;

  // 3. Serialize and Deserialize
  [[maybe_unused]] std::stringstream ss;
  policy.serialize(ss);
  [[maybe_unused]] auto deserialized_res = Policy::deserialize(ss);
  if (!expect(deserialized_res.has_value(), "deserialize failed")) return 1;
  [[maybe_unused]] Policy roundtrip = std::move(deserialized_res.value());

  // 4. Evaluate with PolicyEngine
  PolicyEngine engine(std::move(roundtrip));

  SyscallContext ctx{};
  ctx.current_tier = 3;
  ctx.instruction_count = 50;
  [[maybe_unused]] auto v1 = engine.evaluate(ctx);
  if (!expect(v1.kind == VerdictKind::Allow, "expected allow at count=50")) return 1;

  ctx.instruction_count = 150;
  [[maybe_unused]] auto v2 = engine.evaluate(ctx);
  if (!expect(v2.kind == VerdictKind::Deny, "expected deny at count=150")) return 1;

  std::cout << "Axion Policy Bytecode test passed!\n";
  return 0;
}
