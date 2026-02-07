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
        float tmp[8];
        _mm256_storeu_ps(tmp, vss);
        for (int k = 0; k < 8; ++k) ss += tmp[k];
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
    for (auto& val : out) {
        val = val / (1.0f + std::exp(-val));
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
        for (int j = 1; j < dim; ++j) if (row[j] > max_val) max_val = row[j];

        float sum = 0.0f;
        for (int j = 0; j < dim; ++j) {
            row[j] = std::exp(row[j] - max_val);
            sum += row[j];
        }
        float inv_sum = 1.0f / sum;
#if defined(__AVX2__)
        __m256 vinv = _mm256_set1_ps(inv_sum);
        int j = 0;
        for (; j <= dim - 8; j += 8) {
            __m256 v = _mm256_loadu_ps(&row[j]);
            v = _mm256_mul_ps(v, vinv);
            _mm256_storeu_ps(&row[j], v);
        }
        for (; j < dim; ++j) row[j] *= inv_sum;
#else
        for (int j = 0; j < dim; ++j) row[j] *= inv_sum;
#endif
    }
    return T729Tensor(x.shape(), std::move(out));
}

} // namespace t81::ops
