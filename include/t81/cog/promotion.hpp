#pragma once

#include <t81/support/expected.hpp>
#include "t81/axion/engine.hpp"
#include "t81/cog/tier.hpp"
#include "t81/cog/tier4/tier4_loop.hpp"

namespace t81::cog {
enum class PromotionError {
  NotEligible,
  AxionDenied,
};

template <typename T>
using Result = std::expected<T, PromotionError>;

Result<TierStatus> try_promote(const TierStatus& status, t81::axion::Engine& engine);

/**
 * @brief Heuristic to decide if a task should be promoted based on Tier 4 reflection.
 */
bool should_promote_to_tier4(const v1::ReflectionTrace& trace);

}  // namespace t81::cog
