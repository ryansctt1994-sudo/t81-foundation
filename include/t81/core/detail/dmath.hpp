#pragma once

#include "t81/core/detail/dmath_types.hpp"
#include "t81/core/detail/dmath_trig.hpp"
#include "t81/core/detail/dmath_logexp.hpp"

namespace t81::core::detail {

// Facade for DFixed based math

template <std::size_t M, std::size_t E>
v1::T81Float<M, E> sin(const v1::T81Float<M, E>& x) {
    if (x.is_nae()) return x;
    if (x.is_inf()) return v1::T81Float<M, E>::nae();
    if (x.is_zero()) return x;

    DFixed val = DFixed::from_float(x);
    DFixed res = detail::sin(val);
    return res.to_float<M, E>();
}

template <std::size_t M, std::size_t E>
v1::T81Float<M, E> cos(const v1::T81Float<M, E>& x) {
    if (x.is_nae()) return x;
    if (x.is_inf()) return v1::T81Float<M, E>::nae();
    if (x.is_zero()) return v1::T81Float<M, E>::from_double(1.0);

    DFixed val = DFixed::from_float(x);
    DFixed res = detail::cos(val);
    return res.to_float<M, E>();
}

template <std::size_t M, std::size_t E>
v1::T81Float<M, E> tan(const v1::T81Float<M, E>& x) {
    if (x.is_nae()) return x;
    if (x.is_inf()) return v1::T81Float<M, E>::nae();
    if (x.is_zero()) return x;

    DFixed val = DFixed::from_float(x);
    DFixed s = detail::sin(val);
    DFixed c = detail::cos(val);

    if (c.is_zero()) return v1::T81Float<M, E>::inf(s.v.sign_trit() == Trit::P);

    DFixed res = s / c; // Deterministic Fixed division
    return res.to_float<M, E>();
}

template <std::size_t M, std::size_t E>
v1::T81Float<M, E> exp(const v1::T81Float<M, E>& x) {
    if (x.is_nae()) return x;
    if (x.is_inf()) {
        return x.is_negative() ? v1::T81Float<M, E>::zero() : x;
    }
    if (x.is_zero()) return v1::T81Float<M, E>::from_double(1.0);

    DFixed val = DFixed::from_float(x);
    DFixed res = detail::exp(val);
    return res.to_float<M, E>();
}

template <std::size_t M, std::size_t E>
v1::T81Float<M, E> log(const v1::T81Float<M, E>& x) {
    if (x.is_nae()) return x;
    if (x.is_inf()) {
        return x.is_negative() ? v1::T81Float<M, E>::nae() : x;
    }
    if (x.is_negative() || x.is_zero()) return v1::T81Float<M, E>::nae();

    DFixed val = DFixed::from_float(x);
    DFixed res = detail::log(val);
    return res.to_float<M, E>();
}

template <std::size_t M, std::size_t E>
v1::T81Float<M, E> sqrt(const v1::T81Float<M, E>& x) {
    if (x.is_nae()) return x;
    if (x.is_negative()) return v1::T81Float<M, E>::nae();
    if (x.is_inf()) return x; // sqrt(inf) = inf
    if (x.is_zero()) return x;

    DFixed val = DFixed::from_float(x);
    DFixed res = detail::sqrt(val);
    return res.to_float<M, E>();
}

} // namespace t81::core::detail
