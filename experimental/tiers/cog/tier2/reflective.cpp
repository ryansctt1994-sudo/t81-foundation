#include "t81/experimental/cog/tier2/reflective.hpp"
#include <sstream>

namespace t81::cog::v2 {

void JustificationChain::add_step(const std::string& step) { steps.push_back(step); }

void ReflectiveFrame::capture_state(std::size_t capture_pc,
                                    const std::vector<std::int64_t>& capture_registers,
                                    const std::string& capture_description) {
  pc = capture_pc;
  registers = capture_registers;
  description = capture_description;
  justification.add_step("Captured state: " + capture_description);
  sealed = false;
  hash = 0;
}

bool ReflectiveFrame::check(const std::string& criteria) const {
  if (description.find(criteria) != std::string::npos) return true;
  for (const auto& step : justification.steps) {
    if (step.find(criteria) != std::string::npos) return true;
  }
  return false;
}

void ReflectiveFrame::trace(std::size_t current_pc, const std::vector<std::int64_t>& regs) {
  if (sealed) return;
  std::ostringstream oss;
  oss << "Trace PC=" << current_pc;
  // Log first few registers for context
  for (size_t i = 0; i < std::min<size_t>(regs.size(), 4); ++i) {
    oss << " R" << i << "=" << regs[i];
  }
  justification.add_step(oss.str());
}

void ReflectiveFrame::seal() {
  if (sealed) return;
  std::uint64_t h = 0xcbf29ce484222325ULL;  // FNV offset basis
  auto mix = [&](const std::string& s) {
    for (char c : s) {
      h ^= static_cast<unsigned char>(c);
      h *= 0x100000001b3ULL;  // FNV prime
    }
  };
  mix(description);
  for (const auto& step : justification.steps) {
    mix(step);
  }
  hash = h;
  sealed = true;
  justification.add_step("Sealed hash=" + std::to_string(h));
}

}  // namespace t81::cog::v2
