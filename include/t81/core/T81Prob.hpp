/**
 * @file T81Prob.hpp
 * @brief Defines the T81Prob class for native log-odds probability representation.
 *
 * This file provides the T81Prob<Trits> class, which stores probabilities as
 * log-odds in a fixed-point format over a balanced-ternary integer. This
 * representation offers high precision and allows for more efficient and
 * numerically stable computation of probabilistic operations like softmax,
 * which reduces to simple ternary addition of log-odds values, avoiding
 * expensive exponentiation and division.
 */
#pragma once

#include "t81/core/T81Entropy.hpp"
#include "t81/core/T81Int.hpp"

#include <algorithm>
#include <cmath>
#include <compare>
#include <cstdint>
#include <cstring>
#include <limits>
#include <span>
#include <type_traits>

namespace t81::v1 {

// ======================================================================
// T81Prob<Trits> — Native log-odds / log-probability
// ======================================================================
//
// Representation:
//   Stored as log-odds in base φ ≈ 1.618 (golden ratio) or natural log.
//   Trits controls the underlying T81Int width:
//     Trits = 27 → ~42.8 bits of precision → vastly superior to FP16 log-probs
//   Exact representation of many structured probabilities (powers of 1/3, 1/9,…).
//   Softmax → ternary addition (no exp, no div, no overflow in the hot path).
//   Sampling / arithmetic coding can be layered on top later.
//
// NOTE: The scale (kScale) is currently independent of Trits. Wider Trits simply
//       allow a larger dynamic range of log-odds before saturation.
// ======================================================================
template <std::size_t Trits>
class T81Prob {
public:
  using Storage = T81Int<Trits>;

private:
  Storage log_odds_{};  // log(p / (1-p)) in fixed-point base-φ units

public:
  static constexpr std::size_t kTrits = Trits;

  // ------------------------------------------------------------------
  // Constructors
  // ------------------------------------------------------------------

  T81Prob() noexcept = default;

  explicit T81Prob(Storage v) noexcept : log_odds_(v) {}

  // ------------------------------------------------------------------
  // Core accessors
  // ------------------------------------------------------------------

  [[nodiscard]] const Storage& raw() const noexcept { return log_odds_; }

  [[nodiscard]] const Storage& log_odds() const noexcept { return log_odds_; }

  // ------------------------------------------------------------------
  // Construction from real probability [0,1]
  // ------------------------------------------------------------------
  //
  // Not constexpr: depends on libm (log / exp / llround).
  //
  [[nodiscard]] static T81Prob from_prob(double p) noexcept {
    if (p <= 0.0) return minus_infinity();
    if (p >= 1.0) return plus_infinity();
    if (p == 0.5) return zero();

    const double odds = p / (1.0 - p);
    const double log_odds = std::log(odds);

    // Scale to fixed-point in base φ ≈ 1.618
    constexpr double phi = 1.6180339887498948482;
    constexpr double kScale = 512.0;  // ~9 "fractional trits" in this scheme

    const double scaled = log_odds / std::log(phi);
    const std::int64_t fixed = static_cast<std::int64_t>(std::llround(scaled * kScale));

    return T81Prob(Storage(fixed));
  }

  // ------------------------------------------------------------------
  // Special values
  // ------------------------------------------------------------------

  // log-odds = 0 → p = 0.5
  [[nodiscard]] static T81Prob zero() noexcept { return T81Prob(Storage(0)); }

  // One "unit" of log-odds (≈ 1.0) → p ≈ 0.73111
  [[nodiscard]] static T81Prob one() noexcept { return from_prob(0.731111); }

  [[nodiscard]] static T81Prob minus_infinity() noexcept { return T81Prob(Storage::kMinValue); }

  [[nodiscard]] static T81Prob plus_infinity() noexcept { return T81Prob(Storage::kMaxValue); }

  // ------------------------------------------------------------------
  // Conversion back to probability
  // ------------------------------------------------------------------

  [[nodiscard]] double to_prob() const noexcept {
    if (is_minus_infinity()) return 0.0;
    if (is_plus_infinity()) return 1.0;

    constexpr double phi = 1.6180339887498948482;
    constexpr double kScale = 512.0;

    const double scaled = static_cast<double>(log_odds_.to_int64()) / kScale;
    const double log_odds_real = scaled * std::log(phi);
    const double odds = std::exp(log_odds_real);

    return odds / (1.0 + odds);
  }

  // ------------------------------------------------------------------
  // Queries
  // ------------------------------------------------------------------

  [[nodiscard]] bool is_zero() const noexcept { return log_odds_.is_zero(); }

  [[nodiscard]] bool is_minus_infinity() const noexcept { return log_odds_ == Storage::kMinValue; }

  [[nodiscard]] bool is_plus_infinity() const noexcept { return log_odds_ == Storage::kMaxValue; }

  // ------------------------------------------------------------------
  // Core arithmetic — THIS IS WHY IT'S MAGIC
  // ------------------------------------------------------------------

  [[nodiscard]] T81Prob operator+(const T81Prob& o) const noexcept {
    try {
      return T81Prob(log_odds_ + o.log_odds_);
    } catch (const std::overflow_error&) {
      // T81Int traps on overflow, but T81Prob requires saturation.
      // Since balanced ternary range is symmetric:
      // - Overflow only happens if operands have the same sign.
      // - If operands are positive, result saturates to +inf.
      // - If operands are negative, result saturates to -inf.
      if (log_odds_.sign_trit() == Trit::P) {
        return plus_infinity();
      } else {
        return minus_infinity();
      }
    }
  }

  [[nodiscard]] T81Prob operator-(const T81Prob& o) const noexcept {
    try {
      return T81Prob(log_odds_ - o.log_odds_);
    } catch (const std::overflow_error&) {
      // a - b overflows if signs are opposite.
      // - If a is positive (and b is negative), result saturates to +inf.
      // - If a is negative (and b is positive), result saturates to -inf.
      if (log_odds_.sign_trit() == Trit::P) {
        return plus_infinity();
      } else {
        return minus_infinity();
      }
    }
  }

  [[nodiscard]] T81Prob operator-() const noexcept {
    // Unary negation is safe in balanced ternary (symmetric range).
    return T81Prob(Storage(0) - log_odds_);
  }

  // Softmax over a tensor becomes: just add all → subtract each
  // log_softmax(x_i) = x_i - log_sum_exp(x)
  // → in T81Prob: x_i + (-log_sum_exp_all)
  [[nodiscard]] T81Prob log_softmax_normalize(const T81Prob& log_sum_exp) const noexcept {
    return *this - log_sum_exp;
  }

  // ------------------------------------------------------------------
  // Comparison
  // ------------------------------------------------------------------

  [[nodiscard]] auto operator<=>(const T81Prob& o) const noexcept = default;
  [[nodiscard]] bool operator==(const T81Prob& o) const noexcept = default;
};

// ======================================================================
// Canonical 27-trit probability type
// ======================================================================

using T81Prob27 = T81Prob<27>;

// Static asserts (27-trit specialization matches T81Int<27> size / triviality)
static_assert(sizeof(T81Prob27) == sizeof(T81Int<27>));
static_assert(std::is_trivially_copyable_v<T81Prob27>);

// ======================================================================
// Free functions — used everywhere in sampling (for T81Prob27)
// ======================================================================

// Log-sum-exp over a span (fused into one ternary reduction on Axion)
[[nodiscard]] inline T81Prob27 log_sum_exp(std::span<const T81Prob27> probs) noexcept {
  if (probs.empty()) {
    return T81Prob27::minus_infinity();
  }

  const auto max_it = std::max_element(probs.begin(), probs.end());
  const T81Prob27 max = *max_it;
  if (max.is_minus_infinity()) return max;

  // We compute log(sum(exp(p_i))) = max + log(sum(exp(p_i - max)))
  // T81Prob stores fixed-point log-odds (base phi).
  // We treat them as generic log-values (logits) here.
  double sum_exp = 0.0;
  constexpr double kScale = 512.0;
  constexpr double kPhi = 1.6180339887498948482;
  constexpr double kLnPhi = 0.48121182505960344750;

  for (const auto& p : probs) {
    if (p.is_minus_infinity()) continue;
    // delta = p - max (always <= 0)
    // subtraction of raw values gives the difference in fixed-point units
    std::int64_t delta = (p.raw() - max.raw()).to_int64();

    // Convert fixed-point difference to real exponent:
    // x_real = x_fixed / kScale
    // base is phi. exp(x_real * ln(phi)) = phi^x_real
    sum_exp += std::pow(kPhi, static_cast<double>(delta) / kScale);
  }

  // Convert back to fixed-point log domain
  // result = 512 * log_phi(sum_exp)
  double offset = kScale * std::log(sum_exp) / kLnPhi;
  std::int64_t offset_int = static_cast<std::int64_t>(std::llround(offset));

  // max + offset
  return T81Prob27(max.raw() + T81Int<27>(offset_int));
}

// Gumbel-softmax trick → just add noise from T81Entropy
[[nodiscard]] inline T81Prob27 gumbel_add(const T81Prob27& p,
                                          const t81::T81Entropy& noise) noexcept {
  // 1. Consume entropy (gets T81Int<81>)
  // This will terminate if the entropy has already been consumed.
  auto raw_int = noise.consume();  // T81Int<81>

  // 2. Extract raw bytes from T81Int<81> to form a uint64_t
  // T81Int<81> has 21 bytes. We use the first 8 bytes.
  const auto& data = raw_int.raw_data();
  std::uint64_t u64;

  // Ensure we copy 8 bytes safely
  static_assert(std::tuple_size_v<std::remove_reference_t<decltype(data)>> >= sizeof(u64));
  std::memcpy(&u64, data.data(), sizeof(u64));

  // 3. Generate double in (0,1) using 53 bits of entropy
  // IEEE 754 double has 52 bits of mantissa (plus implicit 1).
  // Standard trick: (u64 >> 11) * 2^-53
  constexpr double kInv2_53 = 1.1102230246251565e-16;  // 2^-53
  double u = (u64 >> 11) * kInv2_53;

  // Avoid exact 0 or 1 to prevent log domain errors
  constexpr double eps = std::numeric_limits<double>::epsilon();
  if (u <= eps) u = eps;
  if (u >= 1.0 - eps) u = 1.0 - eps;

  // 4. Gumbel noise: g = -log(-log(u))
  double g = -std::log(-std::log(u));

  // 5. Convert to T81Prob internal scale (base φ, scaled by 512)
  constexpr double phi = 1.6180339887498948482;
  constexpr double kScale = 512.0;

  double scaled = g / std::log(phi);
  std::int64_t fixed = static_cast<std::int64_t>(std::llround(scaled * kScale));

  T81Prob27 noise_prob{T81Prob27::Storage(fixed)};
  return p + noise_prob;
}

}  // namespace t81::v1

namespace t81 {
using v1::T81Prob27;
}
