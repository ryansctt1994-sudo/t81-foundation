/**
 * @file T81BigInt.hpp
 * @brief T81BigInt — high-level ternary integer wrapper backed by T81Int.
 *
 * Design notes:
 *   • Internally stores a sign bit and one or more T81Int "limbs".
 *   • Current implementation is intentionally conservative: it only supports
 *     values that fit safely in a signed 64-bit integer and delegates all
 *     arithmetic to int64_t, using T81Int as the underlying storage.
 *   • This makes T81BigInt a safe, future-extensible façade: the API is
 *     arbitrary-precision–friendly, while the implementation remains
 *     simple and well-defined for now.
 */

#pragma once

#include "t81/core/T81Int.hpp"

#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>
#include <algorithm>

namespace t81 {

class T81BigInt {
public:
    using size_type = std::size_t;
    static constexpr size_type kLimbTrits = 81;

    using Limb = T81Int<kLimbTrits>;

private:
    // Invariant (current implementation):
    //   • limbs_.size() >= 1
    //   • limbs_[0] encodes the entire magnitude and is non-negative
    //   • For zero, limbs_[0] == 0 and negative_ == false
    std::vector<Limb> limbs_;
    bool negative_ = false;

    void normalize() {
        if (limbs_.empty()) {
            limbs_.emplace_back(0);
            negative_ = false;
            return;
        }

        // Trim leading zero limbs (except the first one if it's the only one)
        while (limbs_.size() > 1 && limbs_.back().is_zero()) {
            limbs_.pop_back();
        }

        if (limbs_.size() == 1 && limbs_[0].is_zero()) {
            negative_ = false;
        }
    }

    void assign_from_int64(std::int64_t v) {
        limbs_.clear();
        if (v < 0) {
            negative_ = true;
            // std::int64_t min value is safe: abs(min) fits in 81 trits.
            const std::int64_t mag = (v == std::numeric_limits<std::int64_t>::min())
                                         ? static_cast<std::int64_t>(1) +
                                               std::numeric_limits<std::int64_t>::max()
                                         : -v;
            limbs_.emplace_back(mag);
        } else {
            negative_ = false;
            limbs_.emplace_back(v);
        }
        normalize();
    }

public:
    // ------------------------------------------------------------------
    // Constructors
    // ------------------------------------------------------------------

    T81BigInt() {
        limbs_.emplace_back(0);
        negative_ = false;
    }

    T81BigInt(const T81BigInt&)            = default;
    T81BigInt(T81BigInt&&) noexcept        = default;
    T81BigInt& operator=(const T81BigInt&) = default;
    T81BigInt& operator=(T81BigInt&&) noexcept = default;

    explicit T81BigInt(std::int64_t v) {
        assign_from_int64(v);
    }

    template <std::size_t N>
    explicit T81BigInt(const T81Int<N>& x) {
        const std::int64_t v = x.to_int64(); // may throw on overflow
        assign_from_int64(v);
    }

    // Factory helpers
    static T81BigInt zero() {
        return T81BigInt(0);
    }

    static T81BigInt from_int64(std::int64_t v) {
        return T81BigInt(v);
    }

    // ------------------------------------------------------------------
    // Int64 conversion
    // ------------------------------------------------------------------

    [[nodiscard]] std::int64_t to_int64() const {
        if (limbs_.empty()) {
            throw std::logic_error("T81BigInt::to_int64: no limbs");
        }
        if (limbs_.size() > 1) {
            throw std::overflow_error("T81BigInt::to_int64: value too large");
        }

        const std::int64_t mag = limbs_[0].to_int64(); // magnitude
        if (mag < 0) {
            // This can happen if the limb was constructed from a negative int64
            // but we want magnitude. Actually assign_from_int64 handles this.
            if (mag == std::numeric_limits<std::int64_t>::min()) {
                 // abs(min) still fits in 81 trits but not in int64_t magnitude.
                 throw std::overflow_error("T81BigInt::to_int64: magnitude overflow");
            }
            return negative_ ? mag : -mag; // should not reach here if normalize is correct
        }

        if (!negative_) {
            return mag;
        }

        return -mag;
    }

    // ------------------------------------------------------------------
    // Basic predicates and helpers
    // ------------------------------------------------------------------

    [[nodiscard]] bool is_zero() const noexcept {
        return limbs_.size() == 1 && limbs_[0].is_zero() && !negative_;
    }

    [[nodiscard]] bool is_negative() const noexcept {
        return negative_ && !is_zero();
    }

    [[nodiscard]] T81BigInt abs() const {
        T81BigInt r = *this;
        r.negative_ = false;
        return r;
    }

    // Balanced-ternary string representation.
    // Digits: '-', '0', '+'
    [[nodiscard]] std::string str() const {
        if (is_zero()) {
            return "0";
        }

        std::string s;
        for (size_t i = 0; i < limbs_.size(); ++i) {
            for (size_t t = 0; t < kLimbTrits; ++t) {
                Trit tr = limbs_[i][t];
                // Magnitude trits are always non-negative in our canonical form
                // after addition/subtraction adjustment.
                if (tr == Trit::P) s.push_back('+');
                else if (tr == Trit::N) s.push_back('-');
                else s.push_back('0');
            }
        }

        // Trim leading zeros (at the end of the string before reverse)
        while (s.size() > 1 && s.back() == '0') s.pop_back();

        if (negative_) {
            s.push_back('-');
        }

        std::reverse(s.begin(), s.end());
        return s;
    }

    // ------------------------------------------------------------------
    // Comparison (multi-limb balanced ternary)
    // ------------------------------------------------------------------

    static int compare_magnitudes(const std::vector<Limb>& a, const std::vector<Limb>& b) {
        if (a.size() != b.size()) {
            return (a.size() < b.size()) ? -1 : 1;
        }
        for (size_t i = a.size(); i-- > 0; ) {
            if (a[i] < b[i]) return -1;
            if (a[i] > b[i]) return 1;
        }
        return 0;
    }

    [[nodiscard]] bool operator==(const T81BigInt& other) const {
        if (is_zero() && other.is_zero()) {
            return true;
        }
        return negative_ == other.negative_ && limbs_ == other.limbs_;
    }

    [[nodiscard]] bool operator!=(const T81BigInt& other) const {
        return !(*this == other);
    }

    [[nodiscard]] bool operator<(const T81BigInt& other) const {
        if (negative_ != other.negative_) {
            return negative_;
        }
        if (is_zero()) return !other.is_zero() && !other.negative_;
        if (other.is_zero()) return negative_;

        int cmp = compare_magnitudes(limbs_, other.limbs_);
        if (negative_) return cmp > 0;
        return cmp < 0;
    }

    [[nodiscard]] bool operator>(const T81BigInt& other) const {
        return other < *this;
    }

    [[nodiscard]] bool operator<=(const T81BigInt& other) const {
        return !(*this > other);
    }

    [[nodiscard]] bool operator>=(const T81BigInt& other) const {
        return !(*this < other);
    }

    // ------------------------------------------------------------------
    // Arithmetic (multi-limb balanced ternary)
    // ------------------------------------------------------------------

    friend T81BigInt operator+(const T81BigInt& a, const T81BigInt& b) {
        T81BigInt res;
        res.limbs_.clear();

        const size_t n = std::max(a.limbs_.size(), b.limbs_.size());
        int carry = 0;
        for (size_t i = 0; i < n || carry != 0; ++i) {
            Limb r;
            for (size_t t = 0; t < kLimbTrits; ++t) {
                int va = 0;
                if (i < a.limbs_.size()) {
                    va = trit_to_int(a.limbs_[i][t]);
                    if (a.negative_) va = -va;
                }
                int vb = 0;
                if (i < b.limbs_.size()) {
                    vb = trit_to_int(b.limbs_[i][t]);
                    if (b.negative_) vb = -vb;
                }
                int sum = va + vb + carry;
                int digit = (sum > 1) ? sum - 3 : (sum < -1) ? sum + 3 : sum;
                carry = (sum > 1) ? 1 : (sum < -1) ? -1 : 0;
                r[t] = int_to_trit(digit);
            }
            res.limbs_.push_back(r);
        }

        // Determine sign from most significant non-zero trit
        Trit s = Trit::Z;
        for (size_t i = res.limbs_.size(); i-- > 0; ) {
            s = res.limbs_[i].sign_trit();
            if (s != Trit::Z) break;
        }

        if (s == Trit::N) {
            res.negative_ = true;
            // Negate to store positive magnitude
            int carry_neg = 0;
            for (size_t i = 0; i < res.limbs_.size(); ++i) {
                for (size_t t = 0; t < kLimbTrits; ++t) {
                    int v = trit_to_int(res.limbs_[i][t]);
                    int val = -v + carry_neg;
                    int digit = (val > 1) ? val - 3 : (val < -1) ? val + 3 : val;
                    carry_neg = (val > 1) ? 1 : (val < -1) ? -1 : 0;
                    res.limbs_[i][t] = int_to_trit(digit);
                }
            }
        } else {
            res.negative_ = false;
        }

        res.normalize();
        return res;
    }

    friend T81BigInt operator-(const T81BigInt& a, const T81BigInt& b) {
        T81BigInt neg_b = b;
        if (!b.is_zero()) neg_b.negative_ = !b.negative_;
        return a + neg_b;
    }

    friend T81BigInt operator*(const T81BigInt& a, const T81BigInt& b) {
        if (a.is_zero() || b.is_zero()) return T81BigInt::zero();

        // Multi-limb schoolbook multiplication for balanced ternary
        T81BigInt res;
        res.limbs_.resize(a.limbs_.size() + b.limbs_.size(), Limb(0));

        // Temporarily ignore signs and multiply magnitudes, but we use the sign-aware trits.
        // Actually, easiest to just do it like standard schoolbook.
        // For simplicity, we can convert trits to absolute and then apply sign.

        for (size_t i = 0; i < a.limbs_.size(); ++i) {
            for (size_t it = 0; it < kLimbTrits; ++it) {
                Trit ta = a.limbs_[i][it];
                if (ta == Trit::Z) continue;

                for (size_t j = 0; j < b.limbs_.size(); ++j) {
                    for (size_t jt = 0; jt < kLimbTrits; ++jt) {
                        Trit tb = b.limbs_[j][jt];
                        if (tb == Trit::Z) continue;

                        // Product of two trits
                        Trit prod = (ta == tb) ? Trit::P : Trit::N;

                        // Add this to res at position (i*81 + it) + (j*81 + jt)
                        size_t pos = (i + j) * kLimbTrits + it + jt;
                        size_t limb_idx = pos / kLimbTrits;
                        size_t trit_idx = pos % kLimbTrits;

                        // Addition with carry propagation
                        int carry = trit_to_int(prod);
                        while (carry != 0) {
                            if (limb_idx >= res.limbs_.size()) res.limbs_.emplace_back(0);
                            int val = trit_to_int(res.limbs_[limb_idx][trit_idx]) + carry;
                            int digit = (val > 1) ? val - 3 : (val < -1) ? val + 3 : val;
                            carry = (val > 1) ? 1 : (val < -1) ? -1 : 0;
                            res.limbs_[limb_idx][trit_idx] = int_to_trit(digit);

                            trit_idx++;
                            if (trit_idx >= kLimbTrits) {
                                trit_idx = 0;
                                limb_idx++;
                            }
                        }
                    }
                }
            }
        }

        res.negative_ = (a.negative_ != b.negative_);

        res.normalize();
        return res;
    }

    T81BigInt& operator+=(const T81BigInt& rhs) {
        *this = *this + rhs;
        return *this;
    }

    T81BigInt& operator-=(const T81BigInt& rhs) {
        *this = *this - rhs;
        return *this;
    }

    T81BigInt& operator*=(const T81BigInt& rhs) {
        *this = *this * rhs;
        return *this;
    }
};

// Convenience alias
using BigInt = T81BigInt;

} // namespace t81::core
