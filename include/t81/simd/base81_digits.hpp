#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace t81::simd::base81_digits {

/**
 * @brief Normalize digits to [0, 80] by performing carry propagation.
 */
int normalize_and_carry(std::span<int16_t> digits);

/**
 * @brief Special normalization for add: handles the extra -40 offset.
 * out[i] = (raw_sums[i] - 40 + carry) normalized.
 */
void normalize_add(std::span<const uint8_t> raw_sums, std::span<uint8_t> out);

/**
 * @brief Special normalization for sub: handles the extra +40 offset.
 * out[i] = (raw_diffs[i] + 40 + carry) normalized.
 */
void normalize_sub(std::span<const int16_t> raw_diffs, std::span<uint8_t> out);

/**
 * @brief Vectorized addition of Base-81 digits.
 * out[i] = a[i] + b[i]
 */
void add(std::span<const uint8_t> a, std::span<const uint8_t> b, std::span<uint8_t> out);

/**
 * @brief Vectorized subtraction of Base-81 digits.
 * out[i] = a[i] - b[i]
 */
void sub(std::span<const uint8_t> a, std::span<const uint8_t> b, std::span<int16_t> out);

/**
 * @brief Add a constant to all digits.
 */
void add_constant(std::span<const uint8_t> in, uint8_t constant, std::span<uint8_t> out);

/**
 * @brief Multiply all digits by a small constant.
 */
void mul_constant(std::span<const uint8_t> in, uint8_t constant, std::span<int16_t> out);

/**
 * @brief Negate Base-81 digits (symbolic negation).
 */
void negate(std::span<const uint8_t> in, std::span<uint8_t> out);

/**
 * @brief Clamp digits to [0, 80].
 */
void clamp(std::span<uint8_t> digits);

/**
 * @brief Compare two Base-81 numbers (lexicographical, from most significant).
 * @return 1 if a > b, -1 if a < b, 0 if a == b.
 */
int compare(std::span<const uint8_t> a, std::span<const uint8_t> b);

}  // namespace t81::simd::base81_digits
