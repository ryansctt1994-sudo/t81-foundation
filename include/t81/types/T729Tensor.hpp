/**
 * @file T729Tensor.hpp
 * @brief T729 (Holotensors) — High-dimensional tensor algebra.
 *
 * Holotensors represent the Base-729 domain ($3^6$), providing high-precision
 * numerical storage and operations aligned with T81Float<72,9>.
 *
 * This implementation leverages the optimized T81Tensor backend while enforcing
 * the specific precision requirements of the Holotensor specification.
 */
#pragma once

#include "t81/types/T81Float.hpp"
#include "t81/types/T81Tensor.hpp"

namespace t81 {

/**
 * @brief The T729 Holotensor type.
 *
 * A Holotensor is a tensor whose elements are 81-trit floating-point numbers
 * (T81Float<72,9>), corresponding to the high-precision domain of T729 ($3^6$ trits effectively).
 *
 * @tparam Rank The rank of the tensor (number of dimensions).
 * @tparam Dims The dimensions of the tensor.
 */
template <size_t Rank, size_t... Dims>
using T729Tensor = T81Tensor<T81Float<72, 9>, Rank, Dims...>;

// Common Holotensor aliases for standard use cases
using HoloScalar = T729Tensor<1, 1>;         // A single high-precision value wrapped in a tensor
using HoloVector = T729Tensor<1, 729>;       // A vector of 729 elements (Base-729 native size)
using HoloMatrix = T729Tensor<2, 729, 729>;  // A matrix of 729x729 elements

namespace holotensor {

/**
 * @brief Creates a Holotensor filled with zeros.
 */
template <size_t Rank, size_t... Dims>
[[nodiscard]] constexpr auto zeros() noexcept -> T729Tensor<Rank, Dims...> {
  return T729Tensor<Rank, Dims...>::zeros();
}

/**
 * @brief Creates a Holotensor filled with a specific value.
 */
template <size_t Rank, size_t... Dims>
[[nodiscard]] constexpr auto fill(T81Float<72, 9> val) noexcept -> T729Tensor<Rank, Dims...> {
  return T729Tensor<Rank, Dims...>(val);
}

/**
 * @brief Contracts two Holotensors (dot product) for Rank 1.
 * Alias for t81::contract but explicitly for Holotensors.
 */
template <size_t N>
[[nodiscard]] constexpr auto contract(const T729Tensor<1, N>& a,
                                      const T729Tensor<1, N>& b) noexcept {
  return t81::contract(a, b);
}

}  // namespace holotensor

}  // namespace t81
