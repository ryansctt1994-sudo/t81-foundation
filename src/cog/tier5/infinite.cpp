#include "t81/cog/tier5/infinite.hpp"
#include <cmath>
#include <string>

namespace t81::cog::v5 {

void InfiniteCanonicalForm::collapse() {
  if (type == SeriesType::Geometric) {
    // Geometric series sum: S = a / (1 - r)
    // Converges if |r| < 1 (i.e., -1 < r < 1)

    auto one = t81::T81Fraction::from_int(1);
    auto neg_one = t81::T81Fraction::from_int(-1);

    // Compare ratio with 1 and -1
    // t81::T81Fraction::cmp(a, b) returns -1, 0, 1
    int cmp_r_one = t81::T81Fraction::cmp(ratio, one);
    int cmp_neg_one_r = t81::T81Fraction::cmp(neg_one, ratio);

    bool r_lt_1 = (cmp_r_one < 0);
    bool neg_1_lt_r = (cmp_neg_one_r < 0);

    bool convergent = r_lt_1 && neg_1_lt_r;

    if (convergent) {
      // denom = 1 - r
      auto denom = t81::T81Fraction::sub(one, ratio);
      // division by zero is impossible since r != 1 (r < 1)

      // Calculate sum = a / (1 - r)
      try {
        sum = t81::T81Fraction::div(first_term, denom);

        // Convert sum to double for signature
        // Using 53 bits mantissa, 11 bits exponent (IEEE 754 double equivalent-ish)
        // Check for zero den just in case
        if (t81::T81BigInt::is_zero(sum.den)) {
          is_convergent = false;
          convergence_signature = "DIVERGENT_ZeroDenom";
        } else {
          auto n = sum.num.to_float<53, 11>().to_double();
          auto d = sum.den.to_float<53, 11>().to_double();
          double s_val = (d == 0.0) ? 0.0 : (n / d);

          is_convergent = true;
          convergence_signature = "GEOMETRIC_SUM_" + std::to_string(s_val);
        }
      } catch (...) {
        is_convergent = false;
        convergence_signature = "DIVERGENT_Error";
      }

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
