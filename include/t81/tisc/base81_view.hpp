#pragma once

#include <string>
#include <vector>
#include "t81/tisc/program.hpp"
#include "t81/core/Result.hpp"

namespace t81::tisc::base81_view {

/**
 * @brief Render a TISC program as a Base-81 symbolic string.
 */
std::string render(const Program& program);

/**
 * @brief Parse a Base-81 symbolic string back into a TISC program.
 */
Result<Program> parse(std::string_view s);

/**
 * @brief Render a single instruction as Base-81 digits.
 */
std::string render_insn(const Insn& insn);

} // namespace t81::tisc::base81_view
