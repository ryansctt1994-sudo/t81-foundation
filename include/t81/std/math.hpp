/**
 * @file math.hpp
 * @brief Deterministic mathematical functions for T81Float.
 *
 * Implements transcendental functions using native T81Float arithmetic
 * to ensure determinism across platforms.
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

// Range reduction helper for trigonometric functions
template <typename T>
T mod_2pi(T x) {
    T p = pi<T>();
    T two_p = p * T::from_double(2.0);
    T div = x / two_p;
    T k = div.round();
    return x - k * two_p;
}

// Sine
template <typename T>
T sin(T x) {
    if (x.is_nae()) return T::nae();
    x = mod_2pi(x);
    // Taylor series: x - x^3/3! + x^5/5! ...
    T term = x;
    T sum = x;
    T x2 = x * x;
    for (int n = 3; n <= 40; n += 2) {
        T denom = T::from_double(static_cast<double>(n * (n-1)));
        term = -term * x2 / denom;
        sum = sum + term;
        if (term.is_zero()) break;
    }
    return sum;
}

// Cosine
template <typename T>
T cos(T x) {
    if (x.is_nae()) return T::nae();
    x = mod_2pi(x);
    // 1 - x^2/2! + x^4/4! ...
    T term = T::from_double(1.0);
    T sum = term;
    T x2 = x * x;
    for (int n = 2; n <= 40; n += 2) {
        T denom = T::from_double(static_cast<double>(n * (n-1)));
        term = -term * x2 / denom;
        sum = sum + term;
        if (term.is_zero()) break;
    }
    return sum;
}

// Tangent
template <typename T>
T tan(T x) {
    return sin(x) / cos(x);
}

// Square root
template <typename T>
T sqrt(T x) {
    if (x.is_nae()) return T::nae();
    if (x.is_negative()) return T::nae();
    if (x.is_zero()) return T::zero();

    // Newton's method
    T y = T::from_double(::std::sqrt(x.to_double()));
    T half = T::from_double(0.5);
    for (int i = 0; i < 15; ++i) {
        y = half * (y + x / y);
    }
    return y;
}

// Exponential
template <typename T>
T exp(T x) {
    if (x.is_nae()) return T::nae();
    if (x.is_negative()) return T::from_double(1.0) / exp(-x);

    // Reduce large x
    int k = 0;
    while (x.to_double() > 1.0) {
        x = x * T::from_double(0.5);
        k++;
    }

    T term = T::from_double(1.0);
    T sum = term;
    for (int n = 1; n <= 40; ++n) {
        term = term * x / T::from_double(static_cast<double>(n));
        sum = sum + term;
        if (term.is_zero()) break;
    }

    // Square k times
    while (k--) {
        sum = sum * sum;
    }

    return sum;
}

// Natural Logarithm
template <typename T>
T log(T x) {
    if (x.is_nae() || x.is_negative() || x.is_zero()) return T::nae();

    // Newton's method on exp(y) = x
    T y = T::from_double(::std::log(x.to_double()));
    T one = T::from_double(1.0);
    for (int i = 0; i < 15; ++i) {
        T ey = exp(y);
        y = y + (x / ey) - one;
    }
    return y;
}

// Power
template <typename T>
T pow(T b, T e) {
    return exp(e * log(b));
}

// Inverse Tangent
template <typename T>
T atan(T x) {
    if (x.is_nae()) return T::nae();

    double xd = x.to_double();
    if (::std::abs(xd) > 1.0) {
        if (x.is_negative()) {
             // atan(x) = -pi/2 - atan(1/x)
             T pi_2 = pi<T>() * T::from_double(0.5);
             return -pi_2 - atan(T::from_double(1.0) / x);
        } else {
             // atan(x) = pi/2 - atan(1/x)
             T pi_2 = pi<T>() * T::from_double(0.5);
             return pi_2 - atan(T::from_double(1.0) / x);
        }
    }

    // Taylor: x - x^3/3 + x^5/5 ...

    T sum = x;
    T term_num = x; // x^1
    T x2 = x * x;
    T neg_x2 = -x2;

    for (int n = 3; n <= 100; n += 2) {
        term_num = term_num * neg_x2; // x^3, -x^5, etc.
        T term = term_num / T::from_double(static_cast<double>(n));
        sum = sum + term;
        if (term.is_zero()) break;
    }

    return sum;
}

template <typename T>
T asin(T x) {
    // asin(x) = atan(x / sqrt(1-x^2))
    double xd = x.to_double();
    if (xd >= 1.0 || xd <= -1.0) {
        // Handle edges carefully?
        if (xd == 1.0) return pi<T>() * T::from_double(0.5);
        if (xd == -1.0) return -pi<T>() * T::from_double(0.5);
        return T::nae();
    }
    T one = T::from_double(1.0);
    return atan(x / sqrt(one - x*x));
}

template <typename T>
T acos(T x) {
    // acos(x) = pi/2 - asin(x)
    T pi_2 = pi<T>() * T::from_double(0.5);
    return pi_2 - asin(x);
}

template <typename T>
T sinh(T x) {
    T ex = exp(x);
    T emx = T::from_double(1.0) / ex;
    return (ex - emx) * T::from_double(0.5);
}

template <typename T>
T cosh(T x) {
    T ex = exp(x);
    T emx = T::from_double(1.0) / ex;
    return (ex + emx) * T::from_double(0.5);
}

template <typename T>
T tanh(T x) {
    return sinh(x) / cosh(x);
}

} // namespace t81::math
