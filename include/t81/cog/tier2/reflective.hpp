#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace t81::cog::v2 {

struct JustificationChain {
  std::vector<std::string> steps;

  void add_step(const std::string& step);
};

struct ReflectiveFrame {
  std::size_t pc{0};
  std::vector<std::int64_t> registers;
  std::string description;
  JustificationChain justification;

  void capture_state(std::size_t pc, const std::vector<std::int64_t>& registers,
                     const std::string& description);
};

}  // namespace t81::cog::v2
