#pragma once

#include <string>

namespace t81::cog {
enum class TierId {
  Tier0,
  Tier1,
  Tier2,
  Tier3,
  Tier4,
  Tier5,
};

struct TierStatus {
  TierId current{TierId::Tier0};
  std::string label;
};
}  // namespace t81::cog

