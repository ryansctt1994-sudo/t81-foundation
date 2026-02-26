#include "t81/experimental/distributed/distributed.hpp"
#include <algorithm>

namespace t81::cog::v4 {

bool CoherenceVector::is_coherent(const CoherenceVector& other) const {
  const uint64_t a = global_tick;
  const uint64_t b = other.global_tick;
  return (a >= b) ? (a - b <= 1) : (b - a <= 1);
}

void NodeState::update_tick() { vector.global_tick++; }

void NodeState::sync_tick(uint64_t remote_tick) {
  if (remote_tick > vector.global_tick) {
    vector.global_tick = remote_tick;
  }
}

void NodeState::gossip(int64_t value, int32_t tag, uint64_t tick, const std::string& sender) {
  sync_tick(tick);
  update_tick();
  outbox.push_back(
      {value, tag, vector.global_tick, sender.empty() ? std::string("local") : sender});
}

std::optional<NetworkMessage> NodeState::merge() {
  if (inbox.empty()) {
    return std::nullopt;
  }

  auto newer_first = [](const NetworkMessage& lhs, const NetworkMessage& rhs) {
    if (lhs.tick != rhs.tick) return lhs.tick > rhs.tick;
    if (lhs.sender != rhs.sender) return lhs.sender < rhs.sender;
    if (lhs.tag != rhs.tag) return lhs.tag < rhs.tag;
    return lhs.payload < rhs.payload;
  };
  std::stable_sort(inbox.begin(), inbox.end(), newer_first);

  auto same_message = [](const NetworkMessage& lhs, const NetworkMessage& rhs) {
    return lhs.payload == rhs.payload && lhs.tag == rhs.tag && lhs.tick == rhs.tick &&
           lhs.sender == rhs.sender;
  };
  inbox.erase(std::unique(inbox.begin(), inbox.end(), same_message), inbox.end());

  NetworkMessage msg = inbox.front();
  inbox.erase(inbox.begin());
  sync_tick(msg.tick);
  update_tick();
  return msg;
}

}  // namespace t81::cog::v4
