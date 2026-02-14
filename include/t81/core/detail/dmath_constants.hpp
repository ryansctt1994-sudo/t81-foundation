#pragma once

#include "t81/core/detail/dmath_types.hpp"

namespace t81::core::detail::constants {

// Pi
template<typename T>
constexpr T pi() {
    return T::from_decimal(3, 1415926535897932384ULL, 19);
}

// Ln2
template<typename T>
constexpr T ln2() {
    return T::from_decimal(0, 6931471805599453094ULL, 19);
}

// Pi/2
template<typename T>
constexpr T pi_2() {
    return T::from_decimal(1, 5707963267948966192ULL, 19);
}

// Pi/4
template<typename T>
constexpr T pi_4() {
    return T::from_decimal(0, 7853981633974483096ULL, 19);
}

// 2*Pi
template<typename T>
constexpr T two_pi() {
    return T::from_decimal(6, 2831853071795864769ULL, 19);
}

// Ln3
template<typename T>
constexpr T ln3() {
    // 1.0986...
    return T::from_decimal(1, 986122886681096913ULL, 19);
}

// Sqrt(3)
template<typename T>
constexpr T sqrt3() {
    return T::from_decimal(1, 7320508075688772935ULL, 19);
}

// Coefficients

template<typename T>
constexpr T half() { return T::from_decimal(0, 5, 1); }

template<typename T>
constexpr T one_sixth() {
    return T::from_decimal(0, 1666666666666666666ULL, 19);
}

template<typename T>
constexpr T one_24th() {
    return T::from_decimal(0, 416666666666666666ULL, 19);
}

template<typename T>
constexpr T one_120th() {
    return T::from_decimal(0, 833333333333333333ULL, 20);
}

template<typename T>
constexpr T one_720th() {
    return T::from_decimal(0, 138888888888888888ULL, 20);
}

template<typename T>
constexpr T one_5040th() {
    return T::from_decimal(0, 198412698412698412ULL, 21);
}

template<typename T>
constexpr T one_40320th() {
    return T::from_decimal(0, 24801587301587301ULL, 21);
}

template<typename T>
constexpr T one_362880th() {
    return T::from_decimal(0, 27557319223985890ULL, 22);
}

template<typename T>
constexpr T one_3628800th() {
    return T::from_decimal(0, 2755731922398589ULL, 22);
}

template<typename T>
constexpr T one_39916800th() {
    return T::from_decimal(0, 2505210838544171ULL, 23);
}

// Helper for brevity in logexp
template<typename T>
constexpr T from_decimal(std::int64_t ip, std::uint64_t fp, std::uint32_t fd) {
    return T::from_decimal(ip, fp, fd);
}

} // namespace t81::core::detail::constants
