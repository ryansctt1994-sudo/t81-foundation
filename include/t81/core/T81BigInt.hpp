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

namespace t81::v1 {

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
            // Use unsigned to avoid overflow warning/UB when negating INT64_MIN.
            const std::uint64_t uv = (v == std::numeric_limits<std::int64_t>::min())
                                         ? static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1
                                         : static_cast<std::uint64_t>(-v);
            limbs_.emplace_back(static_cast<std::int64_t>(uv));
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

        // Optimized limb-based multiplication using 27-trit chunks.
        // Each 81-trit limb is split into 3 chunks of 27 trits each.
        // 3^27 = 7,625,597,484,987, fits in int64_t.
        // Product of two 27-trit chunks fits in __int128.

        auto get_chunks = [](const T81BigInt& x) {
            std::vector<int64_t> chunks;
            chunks.reserve(x.limbs_.size() * 3);
            for (const auto& limb : x.limbs_) {
                for (int c = 0; c < 3; ++c) {
                    int64_t val = 0;
                    int64_t pow3 = 1;
                    for (int t = 0; t < 27; ++t) {
                        val += trit_to_int(limb[c * 27 + t]) * pow3;
                        if (t < 26) pow3 *= 3;
                    }
                    chunks.push_back(val);
                }
            }
            return chunks;
        };

        std::vector<int64_t> ac = get_chunks(a);
        std::vector<int64_t> bc = get_chunks(b);
        std::vector<__int128> rc(ac.size() + bc.size(), 0);

        for (size_t i = 0; i < ac.size(); ++i) {
            if (ac[i] == 0) continue;
            for (size_t j = 0; j < bc.size(); ++j) {
                if (bc[j] == 0) continue;
                rc[i + j] += (__int128)ac[i] * bc[j];
            }
        }

        const __int128 B = 7625597484987LL; // 3^27
        const __int128 halfB = (B - 1) / 2;
        __int128 carry = 0;
        std::vector<int64_t> final_c;
        for (size_t i = 0; i < rc.size() || carry != 0; ++i) {
            __int128 val = (i < rc.size() ? rc[i] : 0) + carry;
            __int128 q = (val >= 0) ? (val + halfB) / B : (val - halfB) / B;
            final_c.push_back(static_cast<int64_t>(val - q * B));
            carry = q;
        }

        T81BigInt res;
        res.limbs_.clear();
        for (size_t i = 0; i < final_c.size(); i += 3) {
            Limb l;
            for (int c = 0; c < 3; ++c) {
                int64_t v = (i + c < final_c.size()) ? final_c[i + c] : 0;
                bool v_neg = v < 0;
                uint64_t uv = v_neg ? static_cast<uint64_t>(-v) : static_cast<uint64_t>(v);
                for (int t = 0; t < 27; ++t) {
                    int r = static_cast<int>(uv % 3); uv /= 3;
                    if (r == 2) { r = -1; uv++; }
                    l[c * 27 + t] = int_to_trit(v_neg ? -r : r);
                }
            }
            res.limbs_.push_back(l);
        }

        res.negative_ = (a.negative_ != b.negative_);

        // Correct sign if magnitude ended up negative (can happen due to balanced ternary representation)
        Trit s = Trit::Z;
        for (size_t i = res.limbs_.size(); i-- > 0; ) {
            s = res.limbs_[i].sign_trit();
            if (s != Trit::Z) break;
        }
        if (s == Trit::N) {
            res.negative_ = !res.negative_;
            int c_neg = 0;
            for (auto& limb : res.limbs_) {
                for (size_t t = 0; t < kLimbTrits; ++t) {
                    int v = -trit_to_int(limb[t]) + c_neg;
                    int d = (v > 1) ? v - 3 : (v < -1) ? v + 3 : v;
                    c_neg = (v > 1) ? 1 : (v < -1) ? -1 : 0;
                    limb[t] = int_to_trit(d);
                }
            }
        }

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

} // namespace t81::v1
