#pragma once

#include <map>
#include <string>
#include <vector>

namespace t81::cog::v1 {

/**
 * @struct SelfModel
 * @brief Represents the internal state of a Tier 4 agent.
 */
struct SelfModel {
  std::string current_goal;
  float confidence{0.0f};
  std::map<std::string, std::string> beliefs;
  std::vector<std::string> history;

  void add_history(const std::string& event) {
    history.push_back(event);
    if (history.size() > 81) {
      history.erase(history.begin());
    }
  }
};

}  // namespace t81::cog::v1
