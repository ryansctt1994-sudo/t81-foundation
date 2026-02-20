#pragma once

#include <cstdint>

namespace t81::cog::v4 {

struct CoherenceVector {
  uint64_t global_tick;

  bool is_coherent(const CoherenceVector& other) const;
};

struct NodeState {
  CoherenceVector vector;

  void update_tick();
};

}  // namespace t81::cog::v4
