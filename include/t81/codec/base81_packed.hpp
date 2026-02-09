#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include "t81/core/Result.hpp"

namespace t81::codec::base81_packed {

/**
 * @brief Pack 5 unbalanced Base-81 digits (0..80) into a single uint32_t.
 *
 * 81^5 = 3,486,784,401 < 2^32 (4,294,967,296)
 */
uint32_t pack5(const uint8_t digits[5]);

/**
 * @brief Unpack a uint32_t into 5 unbalanced Base-81 digits.
 */
void unpack5(uint32_t packed, uint8_t digits[5]);

/**
 * @brief Pack a stream of Base-81 digits into packed32 blocks.
 */
std::vector<uint32_t> pack_stream(std::span<const uint8_t> digits);

/**
 * @brief Unpack a stream of packed32 blocks into Base-81 digits.
 */
std::vector<uint8_t> unpack_stream(std::span<const uint32_t> blocks, size_t original_count);

} // namespace t81::codec::base81_packed
