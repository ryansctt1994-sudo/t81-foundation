#pragma once

#include <string>
#include <vector>

#include "t81/core/T81Fraction.hpp"

namespace t81::cog::v5 {

enum class SeriesType {
  Unknown,
  Geometric
};

struct InfiniteCanonicalForm {
  bool is_convergent = false;
  std::string convergence_signature;

  SeriesType type = SeriesType::Unknown;

  // For Geometric Series: sum_{n=0}^\infty a * r^n
  t81::v1::T81Frac81 first_term;  // a
  t81::v1::T81Frac81 ratio;       // r
  t81::v1::T81Frac81 sum;         // S = a / (1 - r)

  void collapse();
};

struct CollapseSignature {
  std::string hash;

  static CollapseSignature generate(const InfiniteCanonicalForm& form);
};

}  // namespace t81::cog::v5
