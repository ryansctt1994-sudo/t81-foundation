#include "t81/tensor.hpp"
#include "t81/tensor/llama.hpp"
#include "t81/tensor/matmul.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

namespace {

using t81::T729Tensor;

T729Tensor reference_matmul(const T729Tensor& A, const T729Tensor& B) {
  const int m = A.shape()[0];
  const int k = A.shape()[1];
  const int n = B.shape()[1];
  std::vector<float> out(static_cast<size_t>(m) * static_cast<size_t>(n), 0.0f);

  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      float sum = 0.0f;
      for (int p = 0; p < k; ++p) {
        sum += A.data()[static_cast<size_t>(i) * k + p] * B.data()[static_cast<size_t>(p) * n + j];
      }
      out[static_cast<size_t>(i) * n + j] = sum;
    }
  }
  return T729Tensor({m, n}, std::move(out));
}

T729Tensor reference_rmsnorm(const T729Tensor& x, const T729Tensor& w, float eps = 1e-6f) {
  const int dim = x.shape().back();
  std::vector<float> out = x.data();
  const float* w_ptr = w.data().data();
  for (size_t i = 0; i < out.size(); i += static_cast<size_t>(dim)) {
    float ss = 0.0f;
    for (int j = 0; j < dim; ++j) {
      const float v = out[i + static_cast<size_t>(j)];
      ss += v * v;
    }
    const float inv = 1.0f / std::sqrt(ss / static_cast<float>(dim) + eps);
    for (int j = 0; j < dim; ++j) {
      out[i + static_cast<size_t>(j)] = out[i + static_cast<size_t>(j)] * inv * w_ptr[j];
    }
  }
  return T729Tensor(x.shape(), std::move(out));
}

void assert_tensor_near(const T729Tensor& a, const T729Tensor& b, float eps) {
  assert(a.shape() == b.shape());
  assert(a.data().size() == b.data().size());
  for (size_t i = 0; i < a.data().size(); ++i) {
    const float da = a.data()[i];
    const float db = b.data()[i];
    const float diff = std::fabs(da - db);
    if (diff > eps) {
      std::cerr << "tensor mismatch at " << i << ": " << da << " vs " << db << " (diff=" << diff
                << ", eps=" << eps << ")\n";
      assert(false);
    }
  }
}

void test_matmul_backend_parity() {
  // Nontrivial shape and values to exercise vectorized/scalar tails.
  T729Tensor A({5, 7});
  T729Tensor B({7, 6});
  for (int i = 0; i < 5 * 7; ++i) {
    A.data()[static_cast<size_t>(i)] = ((i % 11) - 5) * 0.125f;
  }
  for (int i = 0; i < 7 * 6; ++i) {
    B.data()[static_cast<size_t>(i)] = ((i % 13) - 6) * 0.0625f;
  }

  const auto optimized = t81::ops::matmul(A, B);
  const auto reference = reference_matmul(A, B);
  assert_tensor_near(optimized, reference, 1e-5f);
}

void test_rmsnorm_backend_parity() {
  T729Tensor x({3, 8});
  T729Tensor w({8});
  for (int i = 0; i < 3 * 8; ++i) {
    x.data()[static_cast<size_t>(i)] = ((i % 9) - 4) * 0.3f;
  }
  for (int i = 0; i < 8; ++i) {
    w.data()[static_cast<size_t>(i)] = 0.5f + static_cast<float>(i) * 0.1f;
  }

  const auto optimized = t81::ops::rmsnorm(x, w);
  const auto reference = reference_rmsnorm(x, w);
  assert_tensor_near(optimized, reference, 1e-5f);
}

}  // namespace

int main() {
  test_matmul_backend_parity();
  test_rmsnorm_backend_parity();
  std::cout << "tensor backend parity test passed\n";
  return 0;
}
