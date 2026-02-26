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
  std::vector<t81::T81Fraction> lazy_terms;

  void seed_lazy_prefix();
  bool expand_lazy(std::size_t steps = 1);
  void collapse();
};

struct CollapseSignature {
  std::string hash;

  static CollapseSignature generate(const InfiniteCanonicalForm& form);
};

}  // namespace t81::cog::v5
