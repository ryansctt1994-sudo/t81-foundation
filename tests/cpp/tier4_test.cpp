#include "t81/cog/tier4/tier4_loop.hpp"
#include "t81/cog/tier4/planner.hpp"
#include "t81/axion/engine.hpp"
#include "t81/axion/policy_engine.hpp"
#include "t81/cog/promotion.hpp"
#include <cassert>
#include <iostream>

int main() {
    [[maybe_unused]] auto engine= t81::axion::make_policy_engine(std::nullopt);
    t81::cog::v1::Tier4Loop loop(*engine);

    std::cout << "Starting Tier 4 Reflection Loop test...\n";

    loop.observe("initial contact");
    assert(loop.get_model().history.size() == 1);

    [[maybe_unused]] auto trace= loop.reflect();
    assert(loop.get_model().current_goal == "initialize");
    assert(trace.goal == "initialize");
    assert(!t81::cog::should_promote_to_tier4(trace));

    // Simulate low confidence
    loop.observe("uncertain environment");
    // We don't have a direct way to set confidence in this stub,
    // but we can check the logic if we were to have it.

    // For the sake of testing the promotion heuristic, let's assume we want to test it.
    [[maybe_unused]] t81::cog::v1::ReflectionTrace low_conf_trace;
    low_conf_trace.confidence = 0.5f;
    low_conf_trace.goal = "recalibrate";
    assert(t81::cog::should_promote_to_tier4(low_conf_trace));

    // Test TierAwarePlanner
    using namespace t81::cog::v1;
    TaskMetadata simple_task{"1", 5, 100, false};
    assert(TierAwarePlanner::select_tier(simple_task) == t81::cog::TierId::Tier1);

    TaskMetadata complex_task{"2", 60, 1000, false};
    assert(TierAwarePlanner::select_tier(complex_task) == t81::cog::TierId::Tier4);

    TaskMetadata reflection_task{"3", 10, 500, true};
    assert(TierAwarePlanner::select_tier(reflection_task) == t81::cog::TierId::Tier4);

    std::cout << "Tier 4 Reflection Loop and Planner tests passed!\n";
    [[maybe_unused]] return 0;
}
