#pragma once

#include <string>
#include <vector>

#include "t81/fraction.hpp"

namespace t81::cog::v5 {

enum class SeriesType { Unknown, Geometric };

struct InfiniteCanonicalForm {
  bool is_convergent = false;
  std::string convergence_signature;

  SeriesType type = SeriesType::Unknown;

  // For Geometric Series: sum_{n=0}^\infty a * r^n
  t81::T81Fraction first_term;  // a
  t81::T81Fraction ratio;       // r
  t81::T81Fraction sum;         // S = a / (1 - r)

  void collapse();
};

struct CollapseSignature {
  std::string hash;

  static CollapseSignature generate(const InfiniteCanonicalForm& form);
};

}  // namespace t81::cog::v5
