#include <cassert>
#include <iostream>
#include <sstream>
#include "t81/axion/policy.hpp"
#include "t81/axion/policy_engine.hpp"

int main() {
    using namespace t81::axion;

    // 1. Create a policy with S-expression DSL
    [[maybe_unused]] std::string dsl= "(policy (tier 3) (max-instructions 100))";
    [[maybe_unused]] auto policy_res= parse_policy(dsl);
    assert(policy_res.has_value());
    [[maybe_unused]] Policy policy= std::move(policy_res.value());

    // 2. Compile to bytecode
    policy.compile_to_bytecode();
    assert(!policy.bytecode.empty());

    // 3. Serialize and Deserialize
    [[maybe_unused]] std::stringstream ss;
    policy.serialize(ss);
    [[maybe_unused]] auto deserialized_res= Policy::deserialize(ss);
    assert(deserialized_res.has_value());
    [[maybe_unused]] Policy roundtrip= std::move(deserialized_res.value());

    // 4. Evaluate with PolicyEngine
    PolicyEngine engine(std::move(roundtrip));

    SyscallContext ctx{};
    ctx.instruction_count = 50;
    [[maybe_unused]] auto v1= engine.evaluate(ctx);
    assert(v1.kind == VerdictKind::Allow);

    ctx.instruction_count = 150;
    [[maybe_unused]] auto v2= engine.evaluate(ctx);
    assert(v2.kind == VerdictKind::Deny);

    std::cout << "Axion Policy Bytecode test passed!\n";
    return 0;
}
