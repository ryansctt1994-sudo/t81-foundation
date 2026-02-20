#include "t81/cog/tier4/distributed.hpp"

namespace t81::cog::v4 {

bool CoherenceVector::is_coherent(const CoherenceVector& other) const {
  return global_tick == other.global_tick;
}

void NodeState::update_tick() {
  vector.global_tick++;
}

} // namespace t81::cog::v4
