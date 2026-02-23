#include <iostream>
#include <string>
#include <vector>
#include "t81/axion/api.hpp"
#include "t81/axion/context.hpp"
#include "t81/axion/policy.hpp"
#include "t81/axion/policy_engine.hpp"

using namespace t81::axion;

int main() {
  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "test_check_tier failure: " << msg << "\n";
      return false;
    }
    return true;
  };

  // Case 1: Requirement satisfied
  {
    Policy p;
    p.tier = 2;  // Required tier 2
    p.compile_to_bytecode();

    PolicyEngine engine(std::move(p));
    SyscallContext ctx;
    ctx.current_tier = 2;  // Current tier 2

    Verdict v = engine.evaluate(ctx);
    if (!expect(v.kind == VerdictKind::Allow, "Tier check failed when requirement met (2 >= 2)"))
      return 1;

    ctx.current_tier = 3;  // Current tier 3
    v = engine.evaluate(ctx);
    if (!expect(v.kind == VerdictKind::Allow,
                "Tier check failed when requirement exceeded (3 >= 2)"))
      return 1;
  }

  // Case 2: Requirement not satisfied
  {
    Policy p;
    p.tier = 3;  // Required tier 3
    p.compile_to_bytecode();

    PolicyEngine engine(std::move(p));
    SyscallContext ctx;
    ctx.current_tier = 2;  // Current tier 2

    Verdict v = engine.evaluate(ctx);
    if (!expect(v.kind == VerdictKind::Deny, "Tier check allowed when requirement not met (2 < 3)"))
      return 1;

    std::string expected_reason = "Tier check failed: current=2 required=3";
    if (!expect(v.reason == expected_reason, "Unexpected reason string")) {
      std::cerr << "Expected: " << expected_reason << "\nActual: " << v.reason << "\n";
      return 1;
    }
  }

  std::cout << "test_check_tier passed\n";
  return 0;
}
