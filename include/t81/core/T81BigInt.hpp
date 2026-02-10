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
#if defined(__AVX2__)
#include <immintrin.h>
#endif

namespace t81::v1 {

__extension__ typedef __int128 int128_t;

namespace detail {
    inline const std::array<int16_t, 256>& get_byte_to_ternary() {
        static const auto table = []() {
            std::array<int16_t, 256> t{};
            for (int i = 0; i < 256; ++i) {
                int val = 0;
                int p3 = 1;
                for (int j = 0; j < 4; ++j) {
                    int u = (i >> (j * 2)) & 0x3;
                    if (u > 2) u = 1; // Treat invalid as Zero
                    val += (u - 1) * p3;
                    p3 *= 3;
                }
                t[i] = static_cast<int16_t>(val);
            }
            return t;
        }();
        return table;
    }

    inline const std::array<uint8_t, 81>& get_ternary_to_packed() {
        static const auto table = []() {
            std::array<uint8_t, 81> t{};
            for (int i = 0; i < 81; ++i) {
                int val = 0;
                int tmp = i;
                for (int j = 0; j < 4; ++j) {
                    int d = tmp % 3;
                    tmp /= 3;
                    val |= (d << (j * 2));
                }
                t[i] = static_cast<uint8_t>(val);
            }
            return t;
        }();
        return table;
    }

    inline const std::array<int32_t, 65536>& get_word_to_ternary() {
        static const auto table = []() {
            std::array<int32_t, 65536> t{};
            const auto& b2t = get_byte_to_ternary();
            for (int i = 0; i < 65536; ++i) {
                int lo = i & 0xFF;
                int hi = (i >> 8) & 0xFF;
                t[i] = static_cast<int32_t>(b2t[lo]) + static_cast<int32_t>(b2t[hi]) * 81;
            }
            return t;
        }();
        return table;
    }
}

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

    static std::vector<int64_t> get_chunks_static(const T81BigInt& x) {
        const auto& wtable = detail::get_word_to_ternary();
        std::vector<int64_t> chunks;
        chunks.reserve(x.limbs_.size() * 3);

        static constexpr int64_t p3_8[] = { 1LL, 6561LL, 43046721LL, 282429536481LL };

        for (const auto& limb : x.limbs_) {
            const auto& data = limb.raw_data();
            const uint8_t* d = data.data();

            auto read_u16 = [](const uint8_t* p) {
                uint16_t val;
                std::memcpy(&val, p, 2);
                return val;
            };

            // Chunk 0: trits 0-26
            int64_t c0 = (int64_t)wtable[read_u16(&d[0])] * p3_8[0] +
                         (int64_t)wtable[read_u16(&d[2])] * p3_8[1] +
                         (int64_t)wtable[read_u16(&d[4])] * p3_8[2];
            int b6 = d[6] & 0x3F;
            int64_t v24_26 = 0;
            int p3 = 1;
            for(int j=0; j<3; ++j) {
                int u = (b6 >> (j*2)) & 0x3;
                if (u > 2) u = 1;
                v24_26 += (u - 1) * p3;
                p3 *= 3;
            }
            c0 += v24_26 * 282429536481LL;
            if (x.negative_) c0 = -c0;
            chunks.push_back(c0);

            // Chunk 1: trits 27-53
            int t27 = (d[6] >> 6) & 0x3;
            if (t27 > 2) t27 = 1;
            int64_t c1 = (int64_t)(t27 - 1);
            c1 += (int64_t)wtable[read_u16(&d[7])] * 3LL;
            c1 += (int64_t)wtable[read_u16(&d[9])] * 19683LL;
            c1 += (int64_t)wtable[read_u16(&d[11])] * 129140163LL;
            int b13_0_3 = d[13] & 0x0F;
            int t52 = (b13_0_3 & 0x3); if (t52 > 2) t52 = 1;
            int t53 = (b13_0_3 >> 2) & 0x3; if (t53 > 2) t53 = 1;
            c1 += (int64_t)(t52 - 1) * 847288609443LL;
            c1 += (int64_t)(t53 - 1) * 2541865828329LL;
            if (x.negative_) c1 = -c1;
            chunks.push_back(c1);

            // Chunk 2: trits 54-80
            int b13_4_7 = (d[13] >> 4) & 0x0F;
            int t54 = (b13_4_7 & 0x3); if (t54 > 2) t54 = 1;
            int t55 = (b13_4_7 >> 2) & 0x3; if (t55 > 2) t55 = 1;
            int64_t c2 = (int64_t)(t54 - 1);
            c2 += (int64_t)(t55 - 1) * 3LL;
            c2 += (int64_t)wtable[read_u16(&d[14])] * 9LL;
            c2 += (int64_t)wtable[read_u16(&d[16])] * 59049LL;
            c2 += (int64_t)wtable[read_u16(&d[18])] * 387420489LL;
            int t80 = d[20] & 0x03; if (t80 > 2) t80 = 1;
            c2 += (int64_t)(t80 - 1) * 2541865828329LL;
            if (x.negative_) c2 = -c2;
            chunks.push_back(c2);
        }
        return chunks;
    }

    friend T81BigInt operator+(const T81BigInt& a, const T81BigInt& b) {
        if (a.is_zero()) return b;
        if (b.is_zero()) return a;
        // Fast path: single-limb values avoid chunk expansion and carry normalization.
        if (a.limbs_.size() == 1 && b.limbs_.size() == 1) {
            try {
                T81BigInt res;
                res.limbs_.clear();
                res.limbs_.push_back(a.negative_ ? -a.limbs_[0] : a.limbs_[0]);
                Limb rhs = b.negative_ ? -b.limbs_[0] : b.limbs_[0];
                res.limbs_[0] = res.limbs_[0] + rhs;
                res.negative_ = false;
                res.normalize();
                return res;
            } catch (const std::overflow_error&) {
                // Fall through to chunk/Karatsuba pipeline for promotion.
            }
        }
        auto ac = get_chunks_static(a);
        auto bc = get_chunks_static(b);
        size_t n = std::max(ac.size(), bc.size());
        size_t n_padded = (n + 3) & ~size_t(3);
        ac.resize(n_padded, 0);
        bc.resize(n_padded, 0);
        static constexpr int64_t B = 7625597484987LL;
        static constexpr int64_t halfB = (B - 1) / 2;
        std::vector<int64_t> rc(n_padded + 1, 0);
        size_t i = 0;
#if defined(__AVX2__)
        for (; i < n_padded; i += 4) {
            __m256i va = _mm256_loadu_si256((const __m256i*)&ac[i]);
            __m256i vb = _mm256_loadu_si256((const __m256i*)&bc[i]);
            __m256i vsum = _mm256_add_epi64(va, vb);
            _mm256_storeu_si256((__m256i*)&rc[i], vsum);
        }
#endif
        for (; i < n; ++i) rc[i] = ac[i] + bc[i];
        int64_t carry = 0;
        for (size_t j = 0; j < rc.size() || carry != 0; ++j) {
            if (j >= rc.size()) rc.push_back(0);
            int64_t sum = rc[j] + carry;
            if (sum > halfB) { rc[j] = sum - B; carry = 1; }
            else if (sum < -halfB) { rc[j] = sum + B; carry = -1; }
            else { rc[j] = sum; carry = 0; }
        }
        T81BigInt res;
        res.limbs_.clear();
        int64_t last = 0;
        for (size_t i = rc.size(); i-- > 0; ) if (rc[i] != 0) { last = rc[i]; break; }
        if (last < 0) {
            res.negative_ = true;
            int64_t c_neg = 0;
            for (auto& v : rc) {
                int64_t val = -v + c_neg;
                if (val > halfB) { v = val - B; c_neg = 1; }
                else if (val < -halfB) { v = val + B; c_neg = -1; }
                else { v = val; c_neg = 0; }
            }
        } else res.negative_ = false;
        const auto& packed_table = detail::get_ternary_to_packed();
        for (size_t i = 0; i < rc.size(); i += 3) {
            Limb l;
            auto& ldata = const_cast<std::array<uint8_t, Limb::kNumBytes>&>(l.raw_data());
            std::fill(ldata.begin(), ldata.end(), 0x55u);
            auto set_chunk_in_limb = [&](int c, int64_t v) {
                static constexpr int64_t offset27 = 3812798742493LL;
                uint64_t uv = static_cast<uint64_t>(v + offset27);
                int start_trit = c * 27;
                for (int j = 0; j < 6; ++j) {
                    int r = uv % 81; uv /= 81;
                    uint8_t packed = packed_table[r];
                    for (int t = 0; t < 4; ++t) {
                        int u = (packed >> (t * 2)) & 0x3;
                        l[start_trit + j * 4 + t] = static_cast<Trit>(u - 1);
                    }
                }
                for(int t=24; t<27; ++t) { int r = uv % 3; uv /= 3; l[start_trit + t] = static_cast<Trit>(r - 1); }
            };
            for (int c = 0; c < 3; ++c) {
                int64_t v = (i + c < rc.size()) ? rc[i + c] : 0;
                if (res.negative_) v = -v;
                set_chunk_in_limb(c, v);
            }
            res.limbs_.push_back(l);
        }
        res.normalize();
        return res;
    }

    friend T81BigInt operator-(const T81BigInt& a, const T81BigInt& b) {
        if (b.is_zero()) return a;
        if (a.is_zero()) {
            T81BigInt res = b;
            res.negative_ = !b.negative_;
            return res;
        }
        // For now, reuse addition logic for simplicity and correctness.
        // Optimization: implement direct subtraction with SIMD if needed.
        T81BigInt neg_b = b;
        neg_b.negative_ = !b.negative_;
        return a + neg_b;
    }

    static std::vector<int128_t> karatsuba_mul_(const std::vector<int64_t>& a, const std::vector<int64_t>& b) {
        size_t n = std::max(a.size(), b.size());
        if (n <= 32) { // Schoolbook threshold
            std::vector<int128_t> res(a.size() + b.size(), 0);
            for (size_t i = 0; i < a.size(); ++i) {
                const int64_t val_a = a[i];
                if (val_a == 0) continue;
                const int128_t a128 = val_a;
                for (size_t j = 0; j < b.size(); ++j) {
                    const int64_t val_b = b[j];
                    if (val_b == 0) continue;
                    res[i + j] += a128 * val_b;
                }
            }
            return res;
        }

        size_t k = n / 2;
        auto split = [k](const std::vector<int64_t>& v) {
            std::vector<int64_t> low, high;
            for (size_t i = 0; i < v.size(); ++i) {
                if (i < k) low.push_back(v[i]);
                else high.push_back(v[i]);
            }
            if (low.empty()) low.push_back(0);
            if (high.empty()) high.push_back(0);
            return std::make_pair(low, high);
        };

        auto [a0, a1] = split(a);
        auto [b0, b1] = split(b);

        auto z0 = karatsuba_mul_(a0, b0);
        auto z2 = karatsuba_mul_(a1, b1);

        auto add_v = [](const std::vector<int64_t>& x, const std::vector<int64_t>& y) {
            size_t nx = x.size(), ny = y.size();
            size_t n = std::max(nx, ny);
            std::vector<int64_t> r(n, 0);
            size_t i = 0;
#if defined(__AVX2__)
            size_t n_simd = std::min(nx, ny) & ~size_t(3);
            for (; i < n_simd; i += 4) {
                __m256i vx = _mm256_loadu_si256((const __m256i*)&x[i]);
                __m256i vy = _mm256_loadu_si256((const __m256i*)&y[i]);
                _mm256_storeu_si256((__m256i*)&r[i], _mm256_add_epi64(vx, vy));
            }
#endif
            for (; i < n; ++i) {
                r[i] = (i < nx ? x[i] : 0) + (i < ny ? y[i] : 0);
            }

            int128_t carry = 0;
            const int128_t B = 7625597484987LL;
            const int128_t halfB = (B - 1) / 2;
            for (size_t j = 0; j < r.size() || carry != 0; ++j) {
                if (j >= r.size()) r.push_back(0);
                int128_t val = r[j] + carry;
                int128_t q = (val >= 0) ? (val + halfB) / B : (val - halfB) / B;
                r[j] = static_cast<int64_t>(val - q * B);
                carry = q;
            }
            return r;
        };

        auto a01 = add_v(a0, a1);
        auto b01 = add_v(b0, b1);
        auto z1 = karatsuba_mul_(a01, b01);

        // z1 = z1 - z0 - z2
        auto sub_v_128 = [](std::vector<int128_t>& x, const std::vector<int128_t>& y) {
            size_t nx = x.size(), ny = y.size();
            if (nx < ny) x.resize(ny, 0);
            size_t i = 0;
#if defined(__AVX2__)
            // Optimization: partial SIMD for subtraction if we had 128-bit SIMD.
            // Since we don't, we'll use unrolling.
            for (; i + 3 < ny; i += 4) {
                x[i] -= y[i];
                x[i+1] -= y[i+1];
                x[i+2] -= y[i+2];
                x[i+3] -= y[i+3];
            }
#endif
            for (; i < ny; ++i) x[i] -= y[i];
        };

        std::vector<int128_t> middle = std::move(z1);
        sub_v_128(middle, z0);
        sub_v_128(middle, z2);

        std::vector<int128_t> res(z2.size() + 2 * k, 0);
        if (res.size() < z0.size()) res.resize(z0.size(), 0);
        if (res.size() < middle.size() + k) res.resize(middle.size() + k, 0);

        for (size_t i = 0; i < z0.size(); ++i) res[i] += z0[i];
        for (size_t i = 0; i < middle.size(); ++i) res[i + k] += middle[i];
        for (size_t i = 0; i < z2.size(); ++i) res[i + 2 * k] += z2[i];

        return res;
    }

    friend T81BigInt operator*(const T81BigInt& a, const T81BigInt& b) {
        if (a.is_zero() || b.is_zero()) return T81BigInt::zero();
        // Fast path: single-limb products are common in language/runtime pipelines.
        if (a.limbs_.size() == 1 && b.limbs_.size() == 1) {
            try {
                T81BigInt res;
                res.limbs_.clear();
                res.limbs_.push_back(a.limbs_[0] * b.limbs_[0]);
                res.negative_ = (a.negative_ != b.negative_);
                res.normalize();
                return res;
            } catch (const std::overflow_error&) {
                // Fall through to chunk/Karatsuba pipeline for promotion.
            }
        }
        auto ac = get_chunks_static(a);
        auto bc = get_chunks_static(b);
        std::vector<int128_t> rc = karatsuba_mul_(ac, bc);

        const int128_t B = 7625597484987LL; // 3^27
        const int128_t halfB = (B - 1) / 2;
        int128_t carry = 0;
        std::vector<int64_t> final_c;
        for (size_t i = 0; i < rc.size() || carry != 0; ++i) {
            int128_t val = (i < rc.size() ? rc[i] : 0) + carry;
            int128_t q = (val >= 0) ? (val + halfB) / B : (val - halfB) / B;
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
