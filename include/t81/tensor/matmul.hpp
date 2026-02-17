#pragma once
#include <stdexcept>
#include <vector>
#include "t81/tensor.hpp"

#if defined(__AVX2__)
#include <immintrin.h>
#endif

namespace t81::ops {

// Optimized matrix multiply: (m×k) · (k×n) → (m×n)
inline T729Tensor matmul(const T729Tensor& A, const T729Tensor& B) {
  if (A.rank() != 2 || B.rank() != 2)
    throw std::invalid_argument("matmul: both inputs must be rank-2");
  const int m = A.shape()[0], kA = A.shape()[1];
  const int kB = B.shape()[0], n = B.shape()[1];
  if (kA != kB) throw std::invalid_argument("matmul: inner dimensions mismatch");

  const auto& a = A.data();
  const auto& b = B.data();
  std::vector<float> c(static_cast<size_t>(m) * static_cast<size_t>(n), 0.0f);

  for (int i = 0; i < m; ++i) {
    const size_t c_row = static_cast<size_t>(i) * n;
    for (int p = 0; p < kA; ++p) {
      const float av = a[static_cast<size_t>(i) * kA + static_cast<size_t>(p)];
      if (av == 0.0f) continue;
      const size_t b_row = static_cast<size_t>(p) * n;

#if defined(__AVX2__)
      const __m256 va = _mm256_set1_ps(av);
      int j = 0;
      // Unroll by 4 for better throughput
      for (; j <= n - 32; j += 32) {
        _mm256_storeu_ps(&c[c_row + j + 0], _mm256_fmadd_ps(va, _mm256_loadu_ps(&b[b_row + j + 0]),
                                                            _mm256_loadu_ps(&c[c_row + j + 0])));
        _mm256_storeu_ps(&c[c_row + j + 8], _mm256_fmadd_ps(va, _mm256_loadu_ps(&b[b_row + j + 8]),
                                                            _mm256_loadu_ps(&c[c_row + j + 8])));
        _mm256_storeu_ps(&c[c_row + j + 16],
                         _mm256_fmadd_ps(va, _mm256_loadu_ps(&b[b_row + j + 16]),
                                         _mm256_loadu_ps(&c[c_row + j + 16])));
        _mm256_storeu_ps(&c[c_row + j + 24],
                         _mm256_fmadd_ps(va, _mm256_loadu_ps(&b[b_row + j + 24]),
                                         _mm256_loadu_ps(&c[c_row + j + 24])));
      }
      for (; j <= n - 8; j += 8) {
        __m256 vb = _mm256_loadu_ps(&b[b_row + j]);
        __m256 vc = _mm256_loadu_ps(&c[c_row + j]);
        vc = _mm256_fmadd_ps(va, vb, vc);
        _mm256_storeu_ps(&c[c_row + j], vc);
      }
      for (; j < n; ++j) {
        c[c_row + j] += av * b[b_row + j];
      }
#else
      for (int j = 0; j < n; ++j) {
        c[c_row + j] += av * b[b_row + j];
      }
#endif
    }
  }

  return T729Tensor({m, n}, std::move(c));
}

}  // namespace t81::ops
