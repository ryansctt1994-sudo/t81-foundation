#pragma once

#include "t81/cog/tier.hpp"
#include "t81/axion/engine.hpp"
#include <string>
#include <vector>
#include <memory>

namespace t81::cog {

/**
 * @class Tier4Loop
 * @brief Represents a Tier 4 self-referential cognitive loop.
 */
class Tier4Loop {
public:
    Tier4Loop(const TierStatus& status, t81::axion::Engine& engine);

    void reflect();
    void cycle();

    TierStatus get_status() const { return status_; }

private:
    TierStatus status_;
    t81::axion::Engine& engine_;
    std::vector<std::string> self_model_;

    void update_self_model();
};

} // namespace t81::cog
