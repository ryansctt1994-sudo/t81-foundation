#include "t81/cog/tier3/recursive.hpp"

namespace t81::cog::v3 {

bool ContractionProof::is_valid() const {
  return verified && (final_entropy <= initial_entropy);
}

bool Recursor::can_recurse() const {
  return current_depth < max_depth;
}

void Recursor::push_frame(const ContractionProof& proof) {
  if (proof.is_valid()) {
    proofs.push_back(proof);
    current_depth++;
  }
}

void Recursor::pop_frame() {
  if (current_depth > 0) {
    proofs.pop_back();
    current_depth--;
  }
}

} // namespace t81::cog::v3
