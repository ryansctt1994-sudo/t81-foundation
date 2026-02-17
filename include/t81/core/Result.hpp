#pragma once

#include "t81/core/T81Result.hpp"

namespace t81 {

// Alias to align the T81Lang surface type `Result[T]` with the core
// implementation `T81Result<T>`.
template <typename T>
using Result = T81Result<T>;

}  // namespace t81
