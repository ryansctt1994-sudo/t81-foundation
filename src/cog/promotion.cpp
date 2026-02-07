#include "t81/cog/promotion.hpp"

#include "t81/axion/context.hpp"

namespace t81::cog {
Result<TierStatus> try_promote(const TierStatus& status, t81::axion::Engine& engine) {
  if (status.current == TierId::Tier5) {
    return PromotionError::NotEligible;
  }

  t81::axion::SyscallContext ctx{{}, "system", "promote", nullptr, {}, 0, t81::tisc::Opcode::Nop};
  auto verdict = engine.evaluate(ctx);
  if (verdict.kind == t81::axion::VerdictKind::Deny) {
    return PromotionError::AxionDenied;
  }

  TierStatus next = status;
  switch (status.current) {
    case TierId::Tier0:
      next.current = TierId::Tier1;
      next.label = "Tier1";
      break;
    case TierId::Tier1:
      next.current = TierId::Tier2;
      next.label = "Tier2";
      break;
    case TierId::Tier2:
      next.current = TierId::Tier3;
      next.label = "Tier3";
      break;
    case TierId::Tier3:
      next.current = TierId::Tier4;
      next.label = "Tier4";
      break;
    case TierId::Tier4:
      next.current = TierId::Tier5;
      next.label = "Tier5";
      break;
    default:
      return PromotionError::NotEligible;
  }
  return next;
}
}  // namespace t81::cog

