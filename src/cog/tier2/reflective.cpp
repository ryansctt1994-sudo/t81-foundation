#include "t81/cog/tier2/reflective.hpp"

namespace t81::cog::v2 {

void JustificationChain::add_step(const std::string& step) { steps.push_back(step); }

void ReflectiveFrame::capture_state(std::size_t capture_pc,
                                    const std::vector<std::int64_t>& capture_registers,
                                    const std::string& capture_description) {
  pc = capture_pc;
  registers = capture_registers;
  description = capture_description;
  justification.add_step("Captured state: " + capture_description);
}

}  // namespace t81::cog::v2
