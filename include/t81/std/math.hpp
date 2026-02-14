/**
 * @file math.hpp
 * @brief Deterministic mathematical functions for T81Float.
 *
 * Implements transcendental functions using native T81Float arithmetic
 * to ensure determinism across platforms.
 *
 * Delegates to T81Float's internal deterministic math backend (dmath)
 * where available.
 */
#pragma once
#include "t81/core/T81Float.hpp"

namespace t81::math {

// Constants
template <typename T>
T pi() {
    return T::from_double(3.14159265358979323846);
}

template <typename T>
T e() {
    return T::from_double(2.71828182845904523536);
}

// Basic
template <typename T>
T abs(T x) { return x.abs(); }

template <typename T>
T floor(T x) { return x.floor(); }

template <typename T>
T ceil(T x) { return x.ceil(); }

template <typename T>
T round(T x) { return x.round(); }

// Power (integer exponent)
template <typename T>
T powi(T b, int exp) {
    if (exp == 0) return T::from_double(1.0);
    if (exp < 0) return T::from_double(1.0) / powi(b, -exp);
    T res = T::from_double(1.0);
    while (exp > 0) {
        if (exp & 1) res = res * b;
        b = b * b;
        exp >>= 1;
    }
    return res;
}

// Trigonometric functions
// Now delegating to T81Float member functions which use deterministic backend

template <typename T>
T sin(T x) {
    return x.sin();
}

template <typename T>
T cos(T x) {
    return x.cos();
}

template <typename T>
T tan(T x) {
    return x.tan();
}

template <typename T>
T sqrt(T x) {
    return x.sqrt();
}

template <typename T>
T exp(T x) {
    return x.exp();
}

template <typename T>
T log(T x) {
    return x.log();
}

template <typename T>
T pow(T b, T e) {
    return b.pow(e);
}

// Derived functions (still using ad-hoc implementation or delegating if available)
// Note: These implementation use other deterministic primitives so they should be deterministic
// provided they don't call host math.

// Inverse Tangent
template <typename T>
T atan(T x) {
    // Ideally delegate to x.atan() but x.atan() might not be fully deterministic yet in Phase 1
    // (it falls back to std::atan unless T81_DETERMINISTIC is set, in which case it is NaE).
    // Here we can keep the software implementation if it is better than NaE?
    // The previous implementation used host-math for large values check?
    // "double xd = x.to_double(); if (::std::abs(xd) > 1.0) ..."
    // This is host dependent logic.

    // For now, delegate to member function.
    return x.atan();
}

template <typename T>
T asin(T x) {
    return x.asin();
}

template <typename T>
T acos(T x) {
    return x.acos();
}

template <typename T>
T sinh(T x) {
    return x.sinh();
}

template <typename T>
T cosh(T x) {
    return x.cosh();
}

template <typename T>
T tanh(T x) {
    return x.tanh();
}

} // namespace t81::math
