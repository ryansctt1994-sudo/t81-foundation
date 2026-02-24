#pragma once

#include <cstddef>
#include <cstdint>
#include <t81/support/expected.hpp>
#include <vector>
#include "t81/isa/program.hpp"

namespace t81::tisc {
enum class EncodingError {
  None = 0,
  Truncated,
  InvalidOpcode,
};

std::vector<std::byte> encode(const Program& program);
std::expected<Program, EncodingError> decode(const std::vector<std::byte>& bytes);
}  // namespace t81::tisc
