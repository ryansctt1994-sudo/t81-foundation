#include "t81/cog/tier2/reflective.hpp"

namespace t81::cog::v2 {

void JustificationChain::add_step(const std::string& step) {
  steps.push_back(step);
}

void ReflectiveFrame::capture_state(const std::string& description) {
  justification.add_step(description);
}

} // namespace t81::cog::v2
