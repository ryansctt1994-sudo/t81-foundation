/**
 * @file fraction.hpp
 * @brief Defines the Fraction struct for representing rational numbers.
 *
 * This file provides a simple struct, `Fraction`, for representing rational
 * numbers. It uses the `BigInt` type for both the numerator and the denominator,
 * allowing for arbitrary-precision fractional values.
 */
#pragma once

#include <string>
#include <utility>
#include "t81/types/bigint.hpp"
#include "t81/fraction.hpp"

namespace t81::core {
// Rational compatibility type backed by canonical formatting semantics.
struct Fraction {
  BigInt numerator{};
  BigInt denominator{1};

  Fraction() = default;
  Fraction(BigInt n, BigInt d) : numerator(std::move(n)), denominator(std::move(d)) {}

  /**
   * @brief Constructs the compatibility fraction from canonical fraction.
   */
  explicit Fraction(const ::t81::T81Fraction& f)
      : numerator(BigInt(f.num)), denominator(BigInt(f.den)) {}

  [[nodiscard]] std::string to_string() const;

  /**
   * @brief Returns whether this value can be represented as canonical fraction.
   *
   * Canonical fraction construction rejects denominator == 0.
   */
  [[nodiscard]] bool is_canonicalizable() const noexcept {
    return !::t81::T81BigInt::is_zero(denominator.canonical());
  }

  /**
   * @brief Converts to canonical fraction (throws if denominator is zero).
   */
  [[nodiscard]] ::t81::T81Fraction canonical() const {
    return ::t81::T81Fraction(numerator.canonical(), denominator.canonical());
  }
};
}  // namespace t81::core
