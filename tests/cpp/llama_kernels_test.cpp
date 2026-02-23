#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "t81/tensor.hpp"
#include "t81/tensor/llama.hpp"

bool approx(float a, float b, float eps = 1e-5f) { return std::abs(a - b) < eps; }

int main() {
  using namespace t81;
  using namespace t81::ops;

  // Test RMSNorm
  {
    T729DynamicTensor x({1, 4}, {1.0f, 2.0f, 3.0f, 4.0f});
    T729DynamicTensor w({4}, {1.0f, 1.0f, 1.0f, 1.0f});
    [[maybe_unused]] auto y = rmsnorm(x, w);
    // ss = (1+4+9+16)/4 = 30/4 = 7.5
    // ss = sqrt(7.5 + 1e-6) ~= 2.7386127
    [[maybe_unused]] float inv_ss = 1.0f / std::sqrt(7.5f + 1e-6f);
    assert(approx(y.data()[0], 1.0f * inv_ss));
    assert(approx(y.data()[1], 2.0f * inv_ss));
    assert(approx(y.data()[2], 3.0f * inv_ss));
    assert(approx(y.data()[3], 4.0f * inv_ss));
    std::cout << "RMSNorm ok\n";
  }

  // Test SiLU
  {
    T729DynamicTensor x({2}, {0.0f, 1.0f});
    [[maybe_unused]] auto y = silu(x);
    // silu(0) = 0 / (1 + exp(0)) = 0
    // silu(1) = 1 / (1 + exp(-1)) ~= 1 / (1 + 0.367879) ~= 0.731058
    assert(approx(y.data()[0], 0.0f));
    assert(approx(y.data()[1], 1.0f / (1.0f + std::exp(-1.0f))));
    std::cout << "SiLU ok\n";
  }

  // Test Softmax
  {
    T729DynamicTensor x({1, 3}, {0.0f, 1.0f, 2.0f});
    [[maybe_unused]] auto y = softmax(x);
    // max = 2
    // exp(0-2), exp(1-2), exp(2-2) = exp(-2), exp(-1), 1
    // sum = exp(-2) + exp(-1) + 1 ~= 0.135335 + 0.367879 + 1 = 1.503214
    [[maybe_unused]] float sum = std::exp(-2.0f) + std::exp(-1.0f) + 1.0f;
    assert(approx(y.data()[0], std::exp(-2.0f) / sum));
    assert(approx(y.data()[1], std::exp(-1.0f) / sum));
    assert(approx(y.data()[2], 1.0f / sum));
    std::cout << "Softmax ok\n";
  }

  return 0;
}
