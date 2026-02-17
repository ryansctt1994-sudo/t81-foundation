#pragma once

#include <string>
#include <vector>
#include "t81/cog/tier.hpp"

namespace t81::cog::v1 {

/**
 * @struct TaskMetadata
 * @brief Metadata for a cognitive task to help in tier planning.
 */
struct TaskMetadata {
  std::string task_id;
  int complexity;      // 1-81 scale
  int resource_limit;  // max instructions or cycles
  bool requires_self_reflection;
};

/**
 * @class TierAwarePlanner
 * @brief Selects the optimal cognitive tier for a task based on its metadata.
 */
class TierAwarePlanner {
public:
  static TierId select_tier(const TaskMetadata& task) {
    if (task.requires_self_reflection || task.complexity > 54) {
      return TierId::Tier4;
    }
    if (task.complexity > 27) {
      return TierId::Tier3;
    }
    if (task.complexity > 9) {
      return TierId::Tier2;
    }
    return TierId::Tier1;
  }
};

}  // namespace t81::cog::v1
