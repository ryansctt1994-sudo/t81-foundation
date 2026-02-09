#pragma once

#include <vector>
#include <cstdint>
#include <string_view>
#include "t81/core/T81Int.hpp"
#include "t81/core/Result.hpp"

namespace t81::codec::trit_packing {

/**
 * @brief Error codes for trit packing and conversion operations.
 */
enum class PackingError {
    InvalidPT5Byte,      ///< Byte > 242 encountered in PT-5 stream.
    InvalidBase81Digit,  ///< Digit > 80 encountered in Base-81 stream.
    InvalidTritValue,    ///< Trit not in {-1, 0, +1}.
    InsufficientData,    ///< Not enough input data to satisfy trit_count.
    TritCountMismatch,   ///< Provided trit_count is inconsistent with buffer sizes.
    EncodingError        ///< General encoding/decoding error.
};

/**
 * @brief Convert PackingError to a human-readable string.
 */
const char* to_string(PackingError err);

/**
 * @brief Pack a sequence of trits into PT-5 bytes.
 *
 * Each byte encodes 5 trits using little-endian ordering:
 * byte = (t0+1) + 3*(t1+1) + 9*(t2+1) + 27*(t3+1) + 81*(t4+1)
 * where t_i are trits in {-1, 0, +1}.
 *
 * @param trits The input trit stream.
 * @return A Result containing PT-5 bytes or a PackingError.
 */
Result<std::vector<uint8_t>> pack_pt5(const std::vector<Trit>& trits);

/**
 * @brief Unpack PT-5 bytes into a sequence of trits.
 *
 * @param bytes The input PT-5 bytes.
 * @param trit_count The number of trits to unpack.
 * @return A Result containing the unpacked trits or a PackingError.
 */
Result<std::vector<Trit>> unpack_pt5(const std::vector<uint8_t>& bytes, size_t trit_count);

/**
 * @brief Pack a sequence of trits into Base-81 digits.
 *
 * Each digit encodes 4 trits using little-endian ordering:
 * digit = (t0+1) + 3*(t1+1) + 9*(t2+1) + 27*(t3+1)
 * where t_i are trits in {-1, 0, +1}.
 *
 * @param trits The input trit stream.
 * @return A Result containing Base-81 digits (0..80) or a PackingError.
 */
Result<std::vector<uint8_t>> pack_base81(const std::vector<Trit>& trits);

/**
 * @brief Unpack Base-81 digits into a sequence of trits.
 *
 * @param digits The input Base-81 digits (0..80).
 * @param trit_count The number of trits to unpack.
 * @return A Result containing the unpacked trits or a PackingError.
 */
Result<std::vector<Trit>> unpack_base81(const std::vector<uint8_t>& digits, size_t trit_count);

/**
 * @brief Direct conversion from PT-5 bytes to Base-81 digits.
 *
 * PT-5 (canonical) -> Base-81 (symbolic/IO).
 *
 * @param pt5_bytes The input PT-5 bytes.
 * @param trit_count The authoritative number of trits.
 * @return A Result containing Base-81 digits or a PackingError.
 */
Result<std::vector<uint8_t>> pt5_to_b81(const std::vector<uint8_t>& pt5_bytes, size_t trit_count);

/**
 * @brief Direct conversion from Base-81 digits to PT-5 bytes.
 *
 * Base-81 (symbolic/IO) -> PT-5 (canonical).
 *
 * @param b81_digits The input Base-81 digits (0..80).
 * @param trit_count The authoritative number of trits.
 * @return A Result containing PT-5 bytes or a PackingError.
 */
Result<std::vector<uint8_t>> b81_to_pt5(const std::vector<uint8_t>& b81_digits, size_t trit_count);

/**
 * @brief Encode Base-81 digits into a human-readable string.
 *
 * Uses the canonical alphabet defined in t81/codec/base81.hpp.
 *
 * @param digits The input Base-81 digits (0..80).
 * @return A string representation.
 */
std::string b81_digits_to_string(const std::vector<uint8_t>& digits);

/**
 * @brief Decode a Base-81 string into digits.
 *
 * @param s The input string.
 * @return A Result containing Base-81 digits or a PackingError.
 */
Result<std::vector<uint8_t>> string_to_b81_digits(std::string_view s);

} // namespace t81::codec::trit_packing
