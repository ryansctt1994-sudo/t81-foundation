#pragma once

#include "t81/core/detail/dmath_types.hpp"
#include "t81/core/detail/dmath_constants.hpp"
#include <utility>

namespace t81::core::detail {

template <typename T>
std::pair<T, T> sin_cos_reduced(T x) {
    // x is in [-pi/4, pi/4]
    T x2 = x * x;

    // Cosine
    T c = T::one();
    c = T::one() - x2 * (constants::half<T>() - x2 * (
        constants::one_24th<T>() - x2 * (
            constants::one_720th<T>() - x2 * (
                constants::one_40320th<T>() - x2 * (
                    constants::one_3628800th<T>()
    )))));

    // Sine
    T s_poly = T::one() - x2 * (constants::one_sixth<T>() - x2 * (
        constants::one_120th<T>() - x2 * (
            constants::one_5040th<T>() - x2 * (
                constants::one_362880th<T>() - x2 * (
                    constants::one_39916800th<T>()
    )))));

    T s = x * s_poly;

    return {s, c};
}

template <typename T>
T sin(T x) {
    // Range reduction
    T pi_2 = constants::pi_2<T>();

    T div = x / pi_2;

    auto round_fixed = [](T val) {
        typename T::Storage s = val.v;
        s >>= T::kFractionalTrits;
        s <<= T::kFractionalTrits;
        return T(s);
    };

    T k_approx = round_fixed(div);
    T r = x - k_approx * pi_2;

    std::int64_t k_int = k_approx.to_int64();
    std::int64_t q = k_int % 4;
    if (q < 0) q += 4;

    auto [s, c] = sin_cos_reduced(r);

    switch (q) {
        case 0: return s;
        case 1: return c;
        case 2: return -s;
        case 3: return -c;
    }
    return T::zero();
}

template <typename T>
T cos(T x) {
    return sin(x + constants::pi_2<T>());
}

} // namespace t81::core::detail
