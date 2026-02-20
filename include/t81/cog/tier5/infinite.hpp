#pragma once

#include <string>
#include <vector>

namespace t81::cog::v5 {

struct InfiniteCanonicalForm {
  bool is_convergent;
  std::string convergence_signature;

  void collapse();
};

struct CollapseSignature {
  std::string hash;

  static CollapseSignature generate(const InfiniteCanonicalForm& form);
};

} // namespace t81::cog::v5
