#pragma once
#include <cstdint>
#include "t81/core/T81Int.hpp"

namespace t81 {

#ifdef _MSC_VER
using uint81_t = std::uint64_t;
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
using uint81_t = unsigned __int128;
#pragma GCC diagnostic pop
#endif

}  // namespace t81
