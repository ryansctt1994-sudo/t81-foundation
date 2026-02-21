#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <string>

namespace t81::cog::v4 {

struct CoherenceVector {
  uint64_t global_tick{0};

  bool is_coherent(const CoherenceVector& other) const;
};

struct NetworkMessage {
  int64_t payload{0};
  int32_t tag{0}; // ValueTag
  uint64_t tick{0};
  std::string sender;
};

struct NodeState {
  CoherenceVector vector;
  std::vector<NetworkMessage> inbox;
  std::vector<NetworkMessage> outbox;

  void update_tick();
  void sync_tick(uint64_t remote_tick);
  void gossip(int64_t value, int32_t tag, uint64_t tick, const std::string& sender = "local");
  std::optional<NetworkMessage> merge();
};

}  // namespace t81::cog::v4
