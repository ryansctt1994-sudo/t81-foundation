#pragma once

#include <cstdint>
#include <vector>
#include <span>
#include "t81/core/Result.hpp"

namespace t81::codec::metadata {

enum class EncodingType : uint8_t {
    PT5 = 1,
    B81_DIGITS = 2,
    B81_TEXT = 3
};

struct Header {
    uint32_t magic;      // 'T81C' (0x43313854)
    uint8_t  version;    // 1
    EncodingType encoding;
    uint16_t flags;
    uint64_t trit_count;
};

static_assert(sizeof(Header) == 16, "Header must be exactly 16 bytes");

/**
 * @brief Wrap an encoded buffer with a canonical header.
 */
std::vector<uint8_t> wrap_encoded_buffer(EncodingType encoding, uint64_t trit_count, std::span<const uint8_t> payload);

/**
 * @brief Unwrap a buffer, extracting the header and payload.
 */
Result<std::pair<Header, std::vector<uint8_t>>> unwrap_encoded_buffer(std::span<const uint8_t> buffer);

} // namespace t81::codec::metadata
