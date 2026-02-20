#include "t81/cog/tier5/infinite.hpp"

namespace t81::cog::v5 {

using t81::v1::T81Frac81;

void InfiniteCanonicalForm::collapse() {
  if (type == SeriesType::Geometric) {
    // Geometric series sum: S = a / (1 - r)
    // Converges if |r| < 1 (i.e., -1 < r < 1)

    T81Frac81 one(1);
    T81Frac81 neg_one(-1);

    bool convergent = (ratio < one) && (neg_one < ratio);

    if (convergent) {
      T81Frac81 denom = one - ratio;
      // division by zero is impossible since r != 1
      sum = first_term / denom;
      is_convergent = true;
      convergence_signature = "GEOMETRIC_SUM_" + std::to_string(sum.to_double()); // Placeholder for serialization
    } else {
      is_convergent = false;
      convergence_signature = "DIVERGENT_GEOMETRIC";
    }
  } else {
    // Default stub behavior for Unknown
    is_convergent = true;
    convergence_signature = "COLLAPSED_INFINITY";
  }
}

CollapseSignature CollapseSignature::generate(const InfiniteCanonicalForm& form) {
  CollapseSignature signature;
  if (form.is_convergent) {
    std::string sig = form.convergence_signature;
    signature.hash.assign(sig.begin(), sig.end());
  } else {
    signature.hash = "DIVERGENT";
  }
  return signature;
}

}  // namespace t81::cog::v5
