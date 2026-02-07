#include "t81/cog/tier4/reflection_loop.hpp"
#include "t81/axion/engine.hpp"
#include "t81/axion/policy_engine.hpp"
#include <cassert>
#include <iostream>

int main() {
    auto engine = t81::axion::make_policy_engine(std::nullopt);
    t81::cog::v1::ReflectionLoop loop(*engine);

    std::cout << "Starting Tier 4 Reflection Loop test...\n";

    loop.observe("initial contact");
    assert(loop.get_model().history.size() == 1);

    loop.reflect();
    assert(loop.get_model().current_goal == "initialize");

    // Simulate low confidence by a direct observation that might cause it
    // (In a more complex implementation, we'd have more model influence)
    loop.observe("uncertain environment");
    loop.reflect(); // Should stay stable if confidence is high
    assert(loop.get_model().current_goal == "initialize");

    std::cout << "Tier 4 Reflection Loop test passed!\n";
    return 0;
}
