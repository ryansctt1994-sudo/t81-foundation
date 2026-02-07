#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include "t81/tensor.hpp"

#if defined(__AVX2__) || defined(__FMA__)
#include <immintrin.h>
#endif

namespace t81::ops {

#if defined(__AVX2__)
// Fast SIMD exponential approximation for AVX2.
// Approximation: exp(x) = 2^(x * log2(e)) = 2^n * 2^f
// n = round(x * log2(e)), f = x * log2(e) - n
inline __m256 simd_exp(__m256 x) {
    const __m256 log2e = _mm256_set1_ps(1.4426950408889634f);
    const __m256 ln2_hi = _mm256_set1_ps(0.6931471805599453f);

    __m256 n = _mm256_round_ps(_mm256_mul_ps(x, log2e), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    __m256 f = _mm256_sub_ps(x, _mm256_mul_ps(n, ln2_hi));

    // 6th degree polynomial approximation for 2^f (actually e^f here)
    // p = 1 + f + f^2/2! + f^3/3! + f^4/4! + f^5/5! + f^6/6!
    const __m256 c1 = _mm256_set1_ps(1.0f);
    const __m256 c2 = _mm256_set1_ps(0.5f);
    const __m256 c3 = _mm256_set1_ps(0.16666666666666666f);
    const __m256 c4 = _mm256_set1_ps(0.041666666666666664f);
    const __m256 c5 = _mm256_set1_ps(0.008333333333333333f);
    const __m256 c6 = _mm256_set1_ps(0.0013888888888888889f);

    __m256 p = _mm256_fmadd_ps(f, c6, c5);
    p = _mm256_fmadd_ps(f, p, c4);
    p = _mm256_fmadd_ps(f, p, c3);
    p = _mm256_fmadd_ps(f, p, c2);
    p = _mm256_fmadd_ps(f, p, c1);
    p = _mm256_fmadd_ps(f, p, c1);

    // 2^n multiplication via integer shift
    __m256i imm0 = _mm256_cvtps_epi32(n);
    imm0 = _mm256_add_epi32(imm0, _mm256_set1_epi32(127));
    imm0 = _mm256_slli_epi32(imm0, 23);
    __m256 pow2n = _mm256_castsi256_ps(imm0);

    return _mm256_mul_ps(p, pow2n);
}
#endif

inline T729Tensor rmsnorm(const T729Tensor& x, const T729Tensor& w, float eps = 1e-6f) {
    if (x.rank() == 0 || w.rank() != 1 || w.shape()[0] != x.shape().back()) {
        throw std::invalid_argument("rmsnorm: shape mismatch");
    }
    int dim = x.shape().back();
    std::vector<float> out = x.data();
    const float* w_ptr = w.data().data();

    for (size_t i = 0; i < out.size(); i += static_cast<size_t>(dim)) {
        float* row = &out[i];
        float ss = 0.0f;
#if defined(__AVX2__) && defined(__FMA__)
        __m256 vss = _mm256_setzero_ps();
        int j = 0;
        for (; j <= dim - 8; j += 8) {
            __m256 v = _mm256_loadu_ps(&row[j]);
            vss = _mm256_fmadd_ps(v, v, vss);
        }
        // Optimized horizontal sum for ss
        __m128 vss_h = _mm_add_ps(_mm256_castps256_ps128(vss), _mm256_extractf128_ps(vss, 1));
        vss_h = _mm_add_ps(vss_h, _mm_movehl_ps(vss_h, vss_h));
        vss_h = _mm_add_ps(vss_h, _mm_shuffle_ps(vss_h, vss_h, _MM_SHUFFLE(1, 1, 1, 1)));
        ss = _mm_cvtss_f32(vss_h);

        for (; j < dim; ++j) ss += row[j] * row[j];
#else
        for (int j = 0; j < dim; ++j) ss += row[j] * row[j];
#endif
        float inv_ss = 1.0f / std::sqrt(ss / dim + eps);

#if defined(__AVX2__)
        __m256 vinv = _mm256_set1_ps(inv_ss);
        int j_norm = 0;
        for (; j_norm <= dim - 8; j_norm += 8) {
            __m256 v = _mm256_loadu_ps(&row[j_norm]);
            __m256 vw = _mm256_loadu_ps(&w_ptr[j_norm]);
            v = _mm256_mul_ps(_mm256_mul_ps(v, vinv), vw);
            _mm256_storeu_ps(&row[j_norm], v);
        }
        for (; j_norm < dim; ++j_norm) row[j_norm] = (row[j_norm] * inv_ss) * w_ptr[j_norm];
#else
        for (int j_norm = 0; j_norm < dim; ++j_norm) row[j_norm] = (row[j_norm] * inv_ss) * w_ptr[j_norm];
#endif
    }
    return T729Tensor(x.shape(), std::move(out));
}

inline T729Tensor silu(const T729Tensor& x) {
    std::vector<float> out = x.data();
    float* data = out.data();
    size_t size = out.size();

    size_t i = 0;
#if defined(__AVX2__)
    __m256 vone = _mm256_set1_ps(1.0f);
    for (; i + 8 <= size; i += 8) {
        __m256 vx = _mm256_loadu_ps(&data[i]);
        __m256 vnegx = _mm256_sub_ps(_mm256_setzero_ps(), vx);
        __m256 vexp = simd_exp(vnegx);
        __m256 vres = _mm256_div_ps(vx, _mm256_add_ps(vone, vexp));
        _mm256_storeu_ps(&data[i], vres);
    }
#endif
    for (; i < size; ++i) {
        data[i] = data[i] / (1.0f + std::exp(-data[i]));
    }
    return T729Tensor(x.shape(), std::move(out));
}

inline T729Tensor softmax(const T729Tensor& x) {
    if (x.rank() == 0) throw std::invalid_argument("softmax: rank 0");
    int dim = x.shape().back();
    std::vector<float> out = x.data();
    for (size_t i = 0; i < out.size(); i += static_cast<size_t>(dim)) {
        float* row = &out[i];
        float max_val = row[0];
#if defined(__AVX2__)
        __m256 vmax = _mm256_set1_ps(max_val);
        int j_max = 0;
        for (; j_max + 8 <= dim; j_max += 8) {
            __m256 v = _mm256_loadu_ps(&row[j_max]);
            vmax = _mm256_max_ps(vmax, v);
        }
        alignas(32) float tmp_max[8];
        _mm256_store_ps(tmp_max, vmax);
        for (int k = 0; k < 8; ++k) if (tmp_max[k] > max_val) max_val = tmp_max[k];
        for (; j_max < dim; ++j_max) if (row[j_max] > max_val) max_val = row[j_max];
#else
        for (int j = 1; j < dim; ++j) if (row[j] > max_val) max_val = row[j];
#endif

        float sum = 0.0f;
#if defined(__AVX2__)
        __m256 vsum = _mm256_setzero_ps();
        const __m256 vmax_v = _mm256_set1_ps(max_val);
        int j_sum = 0;
        for (; j_sum + 8 <= dim; j_sum += 8) {
            __m256 vx = _mm256_loadu_ps(&row[j_sum]);
            __m256 vexp = simd_exp(_mm256_sub_ps(vx, vmax_v));
            _mm256_storeu_ps(&row[j_sum], vexp);
            vsum = _mm256_add_ps(vsum, vexp);
        }
        // Horizontal sum of vsum
        __m128 vsum_h = _mm_add_ps(_mm256_castps256_ps128(vsum), _mm256_extractf128_ps(vsum, 1));
        vsum_h = _mm_add_ps(vsum_h, _mm_movehl_ps(vsum_h, vsum_h));
        vsum_h = _mm_add_ps(vsum_h, _mm_shuffle_ps(vsum_h, vsum_h, _MM_SHUFFLE(1, 1, 1, 1)));
        sum = _mm_cvtss_f32(vsum_h);

        for (; j_sum < dim; ++j_sum) {
            row[j_sum] = std::exp(row[j_sum] - max_val);
            sum += row[j_sum];
        }
#else
        for (int j = 0; j < dim; ++j) {
            row[j] = std::exp(row[j] - max_val);
            sum += row[j];
        }
#endif

        float inv_sum = 1.0f / sum;
#if defined(__AVX2__)
        __m256 vinv = _mm256_set1_ps(inv_sum);
        int j_norm = 0;
        for (; j_norm + 8 <= dim; j_norm += 8) {
            __m256 v = _mm256_loadu_ps(&row[j_norm]);
            v = _mm256_mul_ps(v, vinv);
            _mm256_storeu_ps(&row[j_norm], v);
        }
        for (; j_norm < dim; ++j_norm) row[j_norm] *= inv_sum;
#else
        for (int j_norm = 0; j_norm < dim; ++j_norm) row[j_norm] *= inv_sum;
#endif
    }
    return T729Tensor(x.shape(), std::move(out));
}

inline T729Tensor rope(const T729Tensor& x, int pos) {
    if (x.rank() < 2) throw std::invalid_argument("rope: rank must be at least 2");
    int head_dim = x.shape().back();
    std::vector<float> data = x.data();
    for (size_t i = 0; i < data.size(); i += static_cast<size_t>(head_dim)) {
        for (int j = 0; j < head_dim; j += 2) {
            float freq = 1.0f / std::pow(10000.0f, static_cast<float>(j) / head_dim);
            float val = static_cast<float>(pos) * freq;
            float f_cos = std::cos(val);
            float f_sin = std::sin(val);
            float v0 = data[i + static_cast<size_t>(j)];
            float v1 = data[i + static_cast<size_t>(j + 1)];
            data[i + static_cast<size_t>(j)] = v0 * f_cos - v1 * f_sin;
            data[i + static_cast<size_t>(j + 1)] = v0 * f_sin + v1 * f_cos;
        }
    }
    return T729Tensor(x.shape(), std::move(data));
}

} // namespace t81::ops
