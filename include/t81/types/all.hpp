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
#include "t81/types/T81BigInt.hpp"
#include "t81/types/T81Complex.hpp"
#include "t81/types/T81Entropy.hpp"
#include "t81/types/T81Float.hpp"
#include "t81/types/T81Int.hpp"
#include "t81/types/T81Prob.hpp"
#include "t81/types/T81Qutrit.hpp"
#include "t81/types/T81String.hpp"
#include "t81/types/T81Symbol.hpp"

// ======================================================================
// Collections & Structures
// ======================================================================
#include "t81/types/DistributedTensor.hpp"
#include "t81/types/T729Tensor.hpp"
#include "t81/types/T81Category.hpp"
#include "t81/types/T81Fixed.hpp"
#include "t81/types/T81Fraction.hpp"
#include "t81/types/T81Graph.hpp"
#include "t81/types/T81List.hpp"
#include "t81/types/T81Map.hpp"
#include "t81/types/T81Matrix.hpp"
#include "t81/types/T81Polynomial.hpp"
#include "t81/types/T81Quaternion.hpp"
#include "t81/types/T81Set.hpp"
#include "t81/types/T81Stream.hpp"
#include "t81/types/T81Tensor.hpp"
#include "t81/types/T81Tree.hpp"
#include "t81/types/T81Vector.hpp"

// ======================================================================
// Cognition & Truth
// ======================================================================
#include "t81/types/T81Agent.hpp"
#include "t81/types/T81Proof.hpp"
#include "t81/types/T81Reflection.hpp"
#include "t81/types/T81Time.hpp"

// ======================================================================
// Additional Types
// ======================================================================
#include "t81/types/Option.hpp"
#include "t81/types/Result.hpp"
#include "t81/types/T81Bytes.hpp"
#include "t81/types/T81Discovery.hpp"
#include "t81/types/T81IOStream.hpp"
#include "t81/types/T81Limb.hpp"
#include "t81/types/T81Maybe.hpp"
#include "t81/types/T81NN.hpp"
#include "t81/types/T81Network.hpp"
#include "t81/types/T81Promise.hpp"
#include "t81/types/T81Result.hpp"
#include "t81/types/T81Symbolic.hpp"
#include "t81/types/T81Thread.hpp"
#include "t81/types/T81Uint.hpp"

// ======================================================================
// Global namespace
// ======================================================================
namespace t81 {

// Type count and generation
inline constexpr size_t type_count = 90;
inline constexpr uint64_t generation = 90;

}  // namespace t81

#endif  // T81_ALL_HPP
