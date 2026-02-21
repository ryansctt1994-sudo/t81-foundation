#pragma once

#include <vector>

namespace t81::cog::v3 {

struct ContractionProof {
  bool verified;
  double initial_entropy;
  double final_entropy;

  bool is_valid() const;
};

struct Recursor {
  int max_depth{81};
  int current_depth{0};
  std::vector<ContractionProof> proofs;

  bool can_recurse() const;
  void push_frame(const ContractionProof& proof);
  void pop_frame();
};

}  // namespace t81::cog::v3
