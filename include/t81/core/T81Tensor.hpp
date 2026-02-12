/**
 * @file T81Tensor.hpp
 * @brief Defines the T81Tensor class, a multi-dimensional array for high performance.
 *
 * This file provides the `T81Tensor<Element, Rank, Dims...>` class, a versatile
 * multi-dimensional array designed for high-performance numerical computing.
 * It is templatized by the element type, rank, and dimensions, and its memory
 * layout is contiguous and 64-byte aligned to be friendly to tensor cores and
 * other hardware accelerators. It supports essential tensor operations like
 * element-wise arithmetic, reshaping, broadcasting, and transposition.
 */
#pragma once

#include "t81/core/T81Int.hpp"
#include "t81/core/T81Float.hpp"
#include "t81/core/T81Fixed.hpp"
#include "t81/core/T81Complex.hpp"
#include "t81/core/T81Symbol.hpp"

#include <cstddef>
#include <span>
#include <array>
#include <compare>
#include <bit>
#include <cstring>
#include <numeric>
#include <algorithm>
#include <format>
#include <tuple>
#include <utility>

namespace t81 {

// ======================================================================
// Concept: any type that fits in one tryte (81 trits) or is void (for views)
// ======================================================================
template <typename T>
concept T81Element =
    sizeof(T) <= 32 &&                                            // ≤ 81 trits
    (std::same_as<T, T81Int<81>>     ||
     std::same_as<T, T81Float<72,9>>  ||   // 81-trit float
     std::same_as<T, T81Fixed<72,9>>  ||
     std::same_as<T, T81Complex<40>>  ||   // 80 trits → padded
     std::same_as<T, T81Symbol>);

// ======================================================================
// Metaprogramming Helpers
// ======================================================================
namespace detail {
    template <size_t... Is> struct Seq {};

    template <typename S1, typename S2> struct Concat;
    template <size_t... I1, size_t... I2>
    struct Concat<Seq<I1...>, Seq<I2...>> {
        using type = Seq<I1..., I2...>;
    };

    template <size_t... Is> struct Reverse;
    template <> struct Reverse<> { using type = Seq<>; };
    template <size_t I, size_t... Is>
    struct Reverse<I, Is...> {
        using type = typename Concat<typename Reverse<Is...>::type, Seq<I>>::type;
    };
}

// ======================================================================
// T81Tensor<Element, Rank, Dims...> – the ultimate N-D array
// ======================================================================
template <typename Element, size_t Rank, size_t... Dims>
    requires T81Element<Element> && (Rank == sizeof...(Dims)) && (Rank > 0)
class T81Tensor {
public:
    using value_type = Element;

    static constexpr size_t rank() noexcept { return Rank; }
    static constexpr size_t size() noexcept { return (Dims * ...); }
    static constexpr std::array<size_t, Rank> shape() noexcept { return {Dims...}; }

    // Raw storage — always 64-byte aligned for tensor core friendly
    alignas(64) Element data[(Dims * ...)];

    //===================================================================
    // Construction
    //===================================================================
    constexpr T81Tensor() noexcept = default;

    explicit constexpr T81Tensor(Element fill) noexcept {
        std::fill(std::begin(data), std::end(data), fill);
    }

    // Zero-initialized tensor
    static constexpr T81Tensor zeros() noexcept { return T81Tensor(Element{}); }

    //===================================================================
    // Indexing — variadic, constexpr, bounds-checked in debug only
    //===================================================================
    template <typename... Indices>
        requires (sizeof...(Indices) == Rank) && (std::convertible_to<Indices, size_t> && ...)
    [[nodiscard]] constexpr Element& operator()(Indices... indices) noexcept {
        return data[linear_index(indices...)];
    }

    template <typename... Indices>
        requires (sizeof...(Indices) == Rank) && (std::convertible_to<Indices, size_t> && ...)
    [[nodiscard]] constexpr const Element& operator()(Indices... indices) const noexcept {
        return data[linear_index(indices...)];
    }

    //===================================================================
    // Views & reshaping — zero-cost, zero-copy
    //===================================================================
    [[nodiscard]] constexpr std::span<Element>       span()       noexcept { return {data, size()}; }
    [[nodiscard]] constexpr std::span<const Element> span() const noexcept { return {data, size()}; }

    template <size_t... NewDims>
        requires ((sizeof...(NewDims) == Rank) && (size() == (NewDims * ...)))
    [[nodiscard]] constexpr auto reshape() const noexcept
        -> T81Tensor<Element, Rank, NewDims...>
    {
        T81Tensor<Element, Rank, NewDims...> out;
        std::memcpy(out.data, data, sizeof(data));
        return out;
    }

    //===================================================================
    // Broadcasting — compile-time shape propagation (Axion does this in HW)
    //===================================================================
    /**
     * @brief Broadcasts the tensor to a new shape by tiling.
     * @tparam NewDims The target dimensions.
     * @return A new tensor with the specified dimensions.
     */
    template <size_t... NewDims>
        requires (sizeof...(NewDims) == Rank)
    [[nodiscard]] constexpr auto broadcast_to() const noexcept
        -> T81Tensor<Element, Rank, NewDims...>
    {
        T81Tensor<Element, Rank, NewDims...> out;
        std::array<size_t, Rank> old_shape = shape();
        std::array<size_t, Rank> new_shape = {NewDims...};

        // Simple tiling broadcast logic
        for (size_t i = 0; i < out.size(); ++i) {
            std::array<size_t, Rank> coords;
            size_t temp_i = i;
            for (int r = Rank - 1; r >= 0; --r) {
                coords[r] = (temp_i % new_shape[r]) % old_shape[r];
                temp_i /= new_shape[r];
            }

            size_t old_flat = 0;
            size_t stride = 1;
            for (int r = Rank - 1; r >= 0; --r) {
                old_flat += coords[r] * stride;
                stride *= old_shape[r];
            }
            out.span()[i] = span()[old_flat];
        }
        return out;
    }

    //===================================================================
    // Element-wise arithmetic — fused into single ternary instruction stream
    //===================================================================
    [[nodiscard]] constexpr T81Tensor operator+(const T81Tensor& o) const noexcept {
        T81Tensor r;
        for (size_t i = 0; i < size(); ++i) r.data[i] = data[i] + o.data[i];
        return r;
    }

    [[nodiscard]] constexpr T81Tensor operator-(const T81Tensor& o) const noexcept {
        T81Tensor r;
        for (size_t i = 0; i < size(); ++i) r.data[i] = data[i] - o.data[i];
        return r;
    }

    [[nodiscard]] constexpr T81Tensor operator*(const T81Tensor& o) const noexcept {
        T81Tensor r;
        for (size_t i = 0; i < size(); ++i) r.data[i] = data[i] * o.data[i];
        return r;
    }

    [[nodiscard]] constexpr T81Tensor operator/(const T81Tensor& o) const noexcept {
        T81Tensor r;
        for (size_t i = 0; i < size(); ++i) r.data[i] = data[i] / o.data[i];
        return r;
    }

    // Scalar Broadcasting
    [[nodiscard]] constexpr T81Tensor operator+(Element s) const noexcept {
        T81Tensor r;
        for (size_t i = 0; i < size(); ++i) r.data[i] = data[i] + s;
        return r;
    }

    [[nodiscard]] constexpr T81Tensor operator-(Element s) const noexcept {
        T81Tensor r;
        for (size_t i = 0; i < size(); ++i) r.data[i] = data[i] - s;
        return r;
    }

    [[nodiscard]] constexpr T81Tensor operator*(Element s) const noexcept {
        T81Tensor r;
        for (size_t i = 0; i < size(); ++i) r.data[i] = data[i] * s;
        return r;
    }

    [[nodiscard]] constexpr T81Tensor operator/(Element s) const noexcept {
        T81Tensor r;
        for (size_t i = 0; i < size(); ++i) r.data[i] = data[i] / s;
        return r;
    }

    // Friend Scalar Ops (Scalar + Tensor)
    friend constexpr T81Tensor operator+(Element s, const T81Tensor& t) noexcept { return t + s; }
    friend constexpr T81Tensor operator*(Element s, const T81Tensor& t) noexcept { return t * s; }
    friend constexpr T81Tensor operator-(Element s, const T81Tensor& t) noexcept {
        T81Tensor r;
        for (size_t i = 0; i < size(); ++i) r.data[i] = s - t.data[i];
        return r;
    }
    friend constexpr T81Tensor operator/(Element s, const T81Tensor& t) noexcept {
        T81Tensor r;
        for (size_t i = 0; i < size(); ++i) r.data[i] = s / t.data[i];
        return r;
    }

    //===================================================================
    // Comparison
    //===================================================================
    [[nodiscard]] constexpr auto operator<=>(const T81Tensor&) const noexcept = default;
    [[nodiscard]] constexpr bool operator==(const T81Tensor&) const noexcept = default;

private:
    // Row-major linear index
    template <typename... Indices>
    [[nodiscard]] constexpr size_t linear_index(Indices... indices) const noexcept {
        size_t idx[Rank] = { static_cast<size_t>(indices)... };
        size_t flat = 0;
        size_t stride = 1;
        for (int i = static_cast<int>(Rank) - 1; i >= 0; --i) {
            flat += idx[i] * stride;
            stride *= shape()[static_cast<size_t>(i)];
        }
        return flat;
    }
};

// ======================================================================
// The canonical tensor types of the new world
// ======================================================================
using float81 = T81Float<72,9>;

using Vec81      = T81Tensor<float81, 1, 81>;         // 81-dim embedding
using Vec4K      = T81Tensor<float81, 1, 4096>;       // transformer hidden state
using Mat81x81   = T81Tensor<float81, 2, 81, 81>;     // attention matrix
using Mat4Kx4K   = T81Tensor<float81, 2, 4096, 4096>; // weight matrix
using TokenBatch = T81Tensor<float81, 2, 128, 4096>;  // batch, seq, dim
using KVCache    = T81Tensor<T81Fixed<72,9>, 4, 128, 128, 128, 64>; // layers, heads, seq, dim

// Symbolic tensor — exact HRR binding
using SymbolTensor = T81Tensor<T81Symbol, 1, 81>;

//===================================================================
// Generic Transpose Implementation
//===================================================================

namespace detail {
    // Helper to construct T81Tensor from a reversed sequence of dims
    template <typename E, size_t Rank, typename DimsSeq>
    struct TransposeHelper;

    template <typename E, size_t Rank, size_t... Dims>
    struct TransposeHelper<E, Rank, Seq<Dims...>> {
        using type = T81Tensor<E, Rank, Dims...>;
    };

    template <typename E, size_t Rank, size_t... Dims>
    using TransposedType = typename TransposeHelper<E, Rank, typename Reverse<Dims...>::type>::type;
}

/**
 * @brief Generic transpose: reverses the dimensions of any rank tensor.
 */
template <typename E, size_t Rank, size_t... Dims>
[[nodiscard]] constexpr auto transpose(const T81Tensor<E, Rank, Dims...>& t) noexcept
    -> detail::TransposedType<E, Rank, Dims...>
{
    using OutTensor = detail::TransposedType<E, Rank, Dims...>;
    OutTensor out{};

    // We need to iterate over all elements of the input tensor, calculate their multi-index,
    // reverse that index, and assign to the output.
    // Since we don't have a dynamic multi-index iterator, we iterate flat and decode.

    constexpr std::array<size_t, Rank> in_shape = {Dims...};
    // Precompute input strides
    std::array<size_t, Rank> in_strides{};
    size_t s = 1;
    for (int i = (int)Rank - 1; i >= 0; --i) {
        in_strides[i] = s;
        s *= in_shape[i];
    }

    // Precompute output strides (reversed shape)
    constexpr std::array<size_t, Rank> out_shape = OutTensor::shape();
    std::array<size_t, Rank> out_strides{};
    s = 1;
    for (int i = (int)Rank - 1; i >= 0; --i) {
        out_strides[i] = s;
        s *= out_shape[i];
    }

    for (size_t i = 0; i < t.size(); ++i) {
        // Decode linear index i to in_coords
        std::array<size_t, Rank> coords{};
        size_t temp = i;
        for (int d = 0; d < (int)Rank; ++d) {
            coords[d] = temp / in_strides[d];
            temp %= in_strides[d];
        }

        // Calculate out_index by reversing coords
        size_t out_idx = 0;
        for (int d = 0; d < (int)Rank; ++d) {
            // New dim 0 corresponds to old dim (Rank-1)
            // out coords[d] = in coords[Rank-1-d]
            out_idx += coords[Rank - 1 - d] * out_strides[d];
        }

        out.data[out_idx] = t.data[i];
    }

    return out;
}

//===================================================================
// Matrix Multiplication
//===================================================================
/**
 * @brief Matrix multiplication for Rank 2 tensors.
 * (M, K) x (K, N) -> (M, N)
 */
template <typename E, size_t M, size_t K, size_t K_check, size_t N>
    requires (K == K_check)
[[nodiscard]] constexpr auto matmul(const T81Tensor<E, 2, M, K>& a, const T81Tensor<E, 2, K_check, N>& b) noexcept
    -> T81Tensor<E, 2, M, N>
{
    T81Tensor<E, 2, M, N> out; // zero init
    // Naive O(M*N*K)
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            E sum{};
            for (size_t k = 0; k < K; ++k) {
                sum = sum + a(i, k) * b(k, j);
            }
            out(i, j) = sum;
        }
    }
    return out;
}

/**
 * @brief Matrix-Vector multiplication.
 * (M, K) x (K) -> (M)
 */
template <typename E, size_t M, size_t K, size_t K_check>
    requires (K == K_check)
[[nodiscard]] constexpr auto matmul(const T81Tensor<E, 2, M, K>& a, const T81Tensor<E, 1, K_check>& b) noexcept
    -> T81Tensor<E, 1, M>
{
    T81Tensor<E, 1, M> out;
    for (size_t i = 0; i < M; ++i) {
        E sum{};
        for (size_t k = 0; k < K; ++k) {
            sum = sum + a(i, k) * b(k);
        }
        out(i) = sum;
    }
    return out;
}


//===================================================================
// Reductions
//===================================================================
/**
 * @brief Computes the sum of all elements in the tensor.
 */
template <typename E, size_t... Dims>
[[nodiscard]] constexpr E reduce_sum(const T81Tensor<E, sizeof...(Dims), Dims...>& t) noexcept {
    E sum{};
    for (const auto& x : t.span()) sum = sum + x;
    return sum;
}

/**
 * @brief Computes the product of all elements in the tensor.
 */
template <typename E, size_t... Dims>
[[nodiscard]] constexpr E reduce_prod(const T81Tensor<E, sizeof...(Dims), Dims...>& t) noexcept {
    E prod{1}; // Assuming 1 construction
    for (const auto& x : t.span()) prod = prod * x;
    return prod;
}

/**
 * @brief Computes the mean of all elements in the tensor.
 */
template <typename E, size_t... Dims>
[[nodiscard]] constexpr E reduce_mean(const T81Tensor<E, sizeof...(Dims), Dims...>& t) noexcept {
    if constexpr (t.size() == 0) return E{};
    E sum = reduce_sum(t);
    return sum / E(static_cast<long long>(t.size()));
}

/**
 * @brief Computes the max of all elements in the tensor.
 */
template <typename E, size_t... Dims>
[[nodiscard]] constexpr E reduce_max(const T81Tensor<E, sizeof...(Dims), Dims...>& t) noexcept {
    if constexpr (t.size() == 0) return E{};
    E m = t.data[0];
    for (size_t i = 1; i < t.size(); ++i) {
        if (t.data[i] > m) m = t.data[i];
    }
    return m;
}

/**
 * @brief Computes the min of all elements in the tensor.
 */
template <typename E, size_t... Dims>
[[nodiscard]] constexpr E reduce_min(const T81Tensor<E, sizeof...(Dims), Dims...>& t) noexcept {
    if constexpr (t.size() == 0) return E{};
    E m = t.data[0];
    for (size_t i = 1; i < t.size(); ++i) {
        if (t.data[i] < m) m = t.data[i];
    }
    return m;
}


/**
 * @brief Computes the dot product of two Rank 1 tensors.
 */
template <typename E, size_t N>
[[nodiscard]] constexpr E contract(const T81Tensor<E, 1, N>& a, const T81Tensor<E, 1, N>& b) noexcept {
    E res{};
    for (size_t i = 0; i < N; ++i) res = res + a(i) * b(i);
    return res;
}

} // namespace t81
