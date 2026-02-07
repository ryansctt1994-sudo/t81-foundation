#pragma once

#include "t81/cog/tier4/self_model.hpp"
#include "t81/axion/engine.hpp"
#include <memory>

namespace t81::cog::v1 {

/**
 * @class ReflectionLoop
 * @brief Implements the Tier 4 observe-reflect-refine cycle.
 */
class ReflectionLoop {
public:
    ReflectionLoop(t81::axion::Engine& engine);

    void observe(const std::string& observation);
    void reflect();
    void refine();

    const SelfModel& get_model() const { return model_; }

private:
    t81::axion::Engine& engine_;
    SelfModel model_;

    void log_reflection(const std::string& reason);
};

} // namespace t81::cog::v1
