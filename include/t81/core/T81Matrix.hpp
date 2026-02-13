/**
 * @file T81Matrix.hpp
 * @brief Defines the T81Matrix class for matrices of ternary-native scalars.
 */
#pragma once

#include "t81/core/T81Int.hpp"
#include "t81/core/T81Float.hpp"
#include "t81/core/T81Fixed.hpp"
#include "t81/core/T81Complex.hpp"
#include <cstddef>
#include <array>
#include <span>
#include <compare>
#include <bit>
#include <cstring>

namespace t81 {

template <typename T>
concept T81TryteScalar =
    std::same_as<T, T81Int<81>>          ||
    std::same_as<T, T81Float<72,9>>       ||
    std::same_as<T, T81Fixed<72,9>>       ||
    std::same_as<T, T81Complex<40>>;

template <typename Scalar, size_t Rows, size_t Cols>
    requires T81TryteScalar<Scalar>
class T81Matrix {
public:
    using value_type      = Scalar;
    using reference       = Scalar&;
    using const_reference = const Scalar&;

    static constexpr size_t rows    = Rows;
    static constexpr size_t cols    = Cols;
    static constexpr size_t size    = Rows * Cols;

    alignas(64) Scalar data[Rows * Cols];

    constexpr T81Matrix() noexcept = default;

    explicit constexpr T81Matrix(Scalar fill) noexcept {
        for (size_t i = 0; i < size; ++i) data[i] = fill;
    }

    [[nodiscard]] constexpr Scalar&       operator()(size_t r, size_t c)       noexcept { return data[r * Cols + c]; }
    [[nodiscard]] constexpr const Scalar& operator()(size_t r, size_t c) const noexcept { return data[r * Cols + c]; }

    [[nodiscard]] constexpr auto transpose() const noexcept
        -> T81Matrix<Scalar, Cols, Rows>
    {
        T81Matrix<Scalar, Cols, Rows> t;
        for (size_t i = 0; i < Rows; ++i)
            for (size_t j = 0; j < Cols; ++j)
                t(j,i) = (*this)(i,j);
        return t;
    }

    [[nodiscard]] constexpr T81Matrix operator+(const T81Matrix& o) const noexcept {
        T81Matrix r; for (size_t i=0;i<size;++i) r.data[i] = data[i] + o.data[i]; return r;
    }
    [[nodiscard]] constexpr T81Matrix operator-(const T81Matrix& o) const noexcept {
        T81Matrix r; for (size_t i=0;i<size;++i) r.data[i] = data[i] - o.data[i]; return r;
    }
    [[nodiscard]] constexpr T81Matrix operator-() const noexcept {
        T81Matrix r; for (size_t i=0;i<size;++i) r.data[i] = -data[i]; return r;
    }

    template <size_t K>
    [[nodiscard]] friend constexpr auto operator*(
        const T81Matrix<Scalar, Rows, K>& A,
        const T81Matrix<Scalar, K,    Cols>& B) noexcept
        -> T81Matrix<Scalar, Rows, Cols>
    {
        T81Matrix<Scalar, Rows, Cols> C(Scalar(0));
        for (size_t i = 0; i < Rows; ++i)
            for (size_t j = 0; j < Cols; ++j) {
                Scalar sum(0);
                for (size_t k = 0; k < K; ++k)
                    sum = sum + A(i,k) * B(k,j);
                C(i,j) = sum;
            }
        return C;
    }

    [[nodiscard]] constexpr T81Matrix operator*(Scalar s) const noexcept {
        T81Matrix r; for (size_t i=0;i<size;++i) r.data[i] = data[i] * s; return r;
    }

    [[nodiscard]] constexpr Scalar determinant() const noexcept
        requires (Rows == Cols)
    {
        if constexpr (Rows == 1) {
            return (*this)(0,0);
        } else if constexpr (Rows == 2) {
            return (*this)(0,0) * (*this)(1,1) - (*this)(0,1) * (*this)(1,0);
        } else if constexpr (Rows == 3) {
            return (*this)(0,0) * ((*this)(1,1)*(*this)(2,2) - (*this)(1,2)*(*this)(2,1)) -
                   (*this)(0,1) * ((*this)(1,0)*(*this)(2,2) - (*this)(1,2)*(*this)(2,0)) +
                   (*this)(0,2) * ((*this)(1,0)*(*this)(2,1) - (*this)(1,1)*(*this)(2,0));
        } else {
             return Scalar{};
        }
    }

    [[nodiscard]] constexpr T81Matrix inverse() const noexcept
        requires (Rows == Cols)
    {
        T81Matrix res;
        Scalar det = determinant();
        // Note: For integer types, division performs floor/truncation.

        if constexpr (Rows == 1) {
             res(0,0) = Scalar(1) / (*this)(0,0);
        } else if constexpr (Rows == 2) {
             res(0,0) =  (*this)(1,1) / det;
             res(0,1) = -(*this)(0,1) / det;
             res(1,0) = -(*this)(1,0) / det;
             res(1,1) =  (*this)(0,0) / det;
        } else if constexpr (Rows == 3) {
             // Transpose of cofactor matrix divided by det
             res(0,0) = ((*this)(1,1)*(*this)(2,2) - (*this)(1,2)*(*this)(2,1)) / det;
             res(0,1) = ((*this)(0,2)*(*this)(2,1) - (*this)(0,1)*(*this)(2,2)) / det;
             res(0,2) = ((*this)(0,1)*(*this)(1,2) - (*this)(0,2)*(*this)(1,1)) / det;

             res(1,0) = ((*this)(1,2)*(*this)(2,0) - (*this)(1,0)*(*this)(2,2)) / det;
             res(1,1) = ((*this)(0,0)*(*this)(2,2) - (*this)(0,2)*(*this)(2,0)) / det;
             res(1,2) = ((*this)(1,0)*(*this)(0,2) - (*this)(0,0)*(*this)(1,2)) / det;

             res(2,0) = ((*this)(1,0)*(*this)(2,1) - (*this)(1,1)*(*this)(2,0)) / det;
             res(2,1) = ((*this)(2,0)*(*this)(0,1) - (*this)(0,0)*(*this)(2,1)) / det;
             res(2,2) = ((*this)(0,0)*(*this)(1,1) - (*this)(1,0)*(*this)(0,1)) / det;
        }
        return res;
    }

    [[nodiscard]] constexpr auto operator<=>(const T81Matrix&) const noexcept = default;
    [[nodiscard]] constexpr bool operator==(const T81Matrix&) const noexcept = default;
};

using float81  = T81Float<72,9>;
using fixed81  = T81Fixed<72,9>;
using sym81    = T81Int<81>;

using Mat4x4   = T81Matrix<float81, 4, 4>;
using Mat3x3   = T81Matrix<float81, 3, 3>;
// Mat81x81 removed to avoid conflict with T81Tensor.hpp

template <typename S, size_t R, size_t C>
[[nodiscard]] constexpr auto transpose(const T81Matrix<S,R,C>& m) noexcept
    -> T81Matrix<S,C,R> { return m.transpose(); }

template <typename S, size_t N>
[[nodiscard]] constexpr T81Matrix<S,N,N> identity() noexcept {
    T81Matrix<S,N,N> I(S(0));
    for (size_t i = 0; i < N; ++i) I(i,i) = S(1);
    return I;
}

} // namespace t81
