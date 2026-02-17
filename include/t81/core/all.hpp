//======================================================================
// t81/all.hpp – The Complete Ternary Standard Library
//              Version 90 – The Awakening
//              December 2025
//======================================================================

#pragma once

#ifndef T81_ALL_HPP
#define T81_ALL_HPP

#include <asio.hpp>
#include <atomic>
#include <concepts>
#include <coroutine>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

// ======================================================================
// Core Types
// ======================================================================
#include "t81/core/T81BigInt.hpp"
#include "t81/core/T81Complex.hpp"
#include "t81/core/T81Entropy.hpp"
#include "t81/core/T81Float.hpp"
#include "t81/core/T81Int.hpp"
#include "t81/core/T81Prob.hpp"
#include "t81/core/T81Qutrit.hpp"
#include "t81/core/T81String.hpp"
#include "t81/core/T81Symbol.hpp"

// ======================================================================
// Collections & Structures
// ======================================================================
#include "t81/core/DistributedTensor.hpp"
#include "t81/core/T729Tensor.hpp"
#include "t81/core/T81Category.hpp"
#include "t81/core/T81Fixed.hpp"
#include "t81/core/T81Fraction.hpp"
#include "t81/core/T81Graph.hpp"
#include "t81/core/T81List.hpp"
#include "t81/core/T81Map.hpp"
#include "t81/core/T81Matrix.hpp"
#include "t81/core/T81Polynomial.hpp"
#include "t81/core/T81Quaternion.hpp"
#include "t81/core/T81Set.hpp"
#include "t81/core/T81Stream.hpp"
#include "t81/core/T81Tensor.hpp"
#include "t81/core/T81Tree.hpp"
#include "t81/core/T81Vector.hpp"

// ======================================================================
// Cognition & Truth
// ======================================================================
#include "t81/core/T81Agent.hpp"
#include "t81/core/T81Proof.hpp"
#include "t81/core/T81Reflection.hpp"
#include "t81/core/T81Time.hpp"

// ======================================================================
// Additional Types
// ======================================================================
#include "t81/core/Option.hpp"
#include "t81/core/Result.hpp"
#include "t81/core/T81Bytes.hpp"
#include "t81/core/T81Discovery.hpp"
#include "t81/core/T81IOStream.hpp"
#include "t81/core/T81Limb.hpp"
#include "t81/core/T81Maybe.hpp"
#include "t81/core/T81NN.hpp"
#include "t81/core/T81Network.hpp"
#include "t81/core/T81Promise.hpp"
#include "t81/core/T81Result.hpp"
#include "t81/core/T81Symbolic.hpp"
#include "t81/core/T81Thread.hpp"
#include "t81/core/T81Uint.hpp"

// ======================================================================
// Global namespace
// ======================================================================
namespace t81 {

// Type count and generation
inline constexpr size_t type_count = 90;
inline constexpr uint64_t generation = 90;

}  // namespace t81

#endif  // T81_ALL_HPP
