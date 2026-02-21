#include "t81/cog/tier4/distributed.hpp"
#include <algorithm>

namespace t81::cog::v4 {

bool CoherenceVector::is_coherent(const CoherenceVector& other) const {
  return global_tick == other.global_tick;
}

void NodeState::update_tick() {
  vector.global_tick++;
}

void NodeState::sync_tick(uint64_t remote_tick) {
  if (remote_tick > vector.global_tick) {
    vector.global_tick = remote_tick;
  }
}

void NodeState::gossip(int64_t value, int32_t tag, uint64_t tick, const std::string& sender) {
  outbox.push_back({value, tag, tick, sender});
}

std::optional<NetworkMessage> NodeState::merge() {
  if (inbox.empty()) {
    return std::nullopt;
  }
  NetworkMessage msg = inbox.front();
  inbox.erase(inbox.begin());
  sync_tick(msg.tick);
  return msg;
}

}  // namespace t81::cog::v4
