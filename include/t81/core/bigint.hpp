/**
 * @file bigint.hpp
 * @brief Defines the BigInt class for arbitrary-precision integers.
 */

#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include "t81/bigint.hpp"

namespace t81::core {

/**
 * @class BigInt
 * @brief Represents a canonical arbitrary-precision integer.
 *
 * This class provides a foundational data type for handling integers that may
 * exceed the limits of standard primitive types. It is used throughout the
 * T81 ecosystem for both numerical calculations and data representation.
 *
 * @note This compatibility façade delegates to `t81::T81BigInt`.
 *       It preserves the legacy `t81::core::BigInt` API while using
 *       the canonical arithmetic implementation path.
 */
class BigInt {
 public:
  /**
   * @brief Default constructor, initializes the BigInt to zero.
   */
  BigInt() = default;

  /**
   * @brief Constructs a BigInt from a 64-bit signed integer.
   * @param v The initial value.
   */
  explicit BigInt(std::int64_t v) : impl_(::t81::T81BigInt::from_i64(v)) {}

  /**
   * @brief Constructs a compatibility wrapper from canonical BigInt.
   */
  explicit BigInt(const ::t81::T81BigInt& v) : impl_(v) {}

  /**
   * @brief Move-constructs a compatibility wrapper from canonical BigInt.
   */
  explicit BigInt(::t81::T81BigInt&& v) : impl_(std::move(v)) {}

  /**
   * @brief Retrieves the underlying value.
   * @return The 64-bit integer value.
   * @note Preserved for compatibility with legacy callers.
   */
  [[nodiscard]] std::int64_t value() const { return impl_.to_int64(); }

  /**
   * @brief Converts the BigInt to its string representation.
   * @return A string representing the integer's value.
   */
  [[nodiscard]] std::string to_string() const;

  /**
   * @brief Adds two compatibility BigInt values via canonical arithmetic.
   */
  [[nodiscard]] static BigInt add(const BigInt& a, const BigInt& b) {
    return BigInt(::t81::T81BigInt::add(a.impl_, b.impl_));
  }

  /**
   * @brief Subtracts two compatibility BigInt values via canonical arithmetic.
   */
  [[nodiscard]] static BigInt sub(const BigInt& a, const BigInt& b) {
    return BigInt(::t81::T81BigInt::sub(a.impl_, b.impl_));
  }

  /**
   * @brief Multiplies two compatibility BigInt values via canonical arithmetic.
   */
  [[nodiscard]] static BigInt mul(const BigInt& a, const BigInt& b) {
    return BigInt(::t81::T81BigInt::mul(a.impl_, b.impl_));
  }

  /**
   * @brief Returns a read-only view of the canonical BigInt implementation.
   */
  [[nodiscard]] const ::t81::T81BigInt& canonical() const noexcept {
    return impl_;
  }

 private:
  ::t81::T81BigInt impl_{};
};

}  // namespace t81::core
