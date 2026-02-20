#pragma once

#include <string>
#include <vector>

namespace t81::cog::v2 {

struct JustificationChain {
  std::vector<std::string> steps;

  void add_step(const std::string& step);
};

struct ReflectiveFrame {
  JustificationChain justification;

  void capture_state(const std::string& description);
};

}  // namespace t81::cog::v2
