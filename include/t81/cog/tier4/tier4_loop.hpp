#pragma once

#include "t81/cog/tier4/self_model.hpp"
#include "t81/axion/engine.hpp"
#include <memory>
#include <string>
#include <vector>

namespace t81::cog::v1 {

/**
 * @struct ReflectionTrace
 * @brief Captures cognitive state for auditing and promotion decisions.
 */
struct ReflectionTrace {
    std::string goal;
    float confidence;
    std::string reason;
    std::vector<std::string> history_snapshot;
};

/**
 * @class Tier4Loop
 * @brief Implements the Tier 4 observe-reflect-refine cycle.
 */
class Tier4Loop {
public:
    Tier4Loop(t81::axion::Engine& engine);

    void observe(const std::string& observation);
    ReflectionTrace reflect();
    void refine();

    const SelfModel& get_model() const { return model_; }

    /**
     * @brief Updates the agent's self-model based on internal reflection.
     */
    void update_self_model(const std::string& belief_key, const std::string& belief_val);

private:
    t81::axion::Engine& engine_;
    SelfModel model_;

    void log_reflection(const std::string& reason);
};

} // namespace t81::cog::v1
