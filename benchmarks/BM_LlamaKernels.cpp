#include <benchmark/benchmark.h>
#include "t81/tensor.hpp"
#include "t81/tensor/matmul.hpp"
#include "t81/tensor/llama.hpp"
#include <vector>
#include <random>

using namespace t81;

static void BM_Llama_RMSNorm_T81(benchmark::State& state) {
    const int hidden_dim = state.range(0);
    T729Tensor x({1, hidden_dim});
    T729Tensor w({hidden_dim});

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (auto& v : x.data()) v = dis(gen);
    for (auto& v : w.data()) v = dis(gen);

    for (auto _ : state) {
        auto res = t81::ops::rmsnorm(x, w);
        benchmark::DoNotOptimize(res);
    }
    state.SetItemsProcessed(state.iterations() * hidden_dim);
}
BENCHMARK(BM_Llama_RMSNorm_T81)->Arg(1024)->Arg(2048)->Arg(4096);

static void BM_Llama_RMSNorm_Binary(benchmark::State& state) {
    const int hidden_dim = state.range(0);
    std::vector<float> x(hidden_dim);
    std::vector<float> w(hidden_dim);

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (auto& v : x) v = dis(gen);
    for (auto& v : w) v = dis(gen);

    for (auto _ : state) {
        float ss = 0;
        for (int i = 0; i < hidden_dim; i++) ss += x[i] * x[i];
        float inv_ss = 1.0f / std::sqrt(ss / hidden_dim + 1e-6f);
        for (int i = 0; i < hidden_dim; i++) x[i] = (x[i] * inv_ss) * w[i];
        benchmark::DoNotOptimize(x);
    }
    state.SetItemsProcessed(state.iterations() * hidden_dim);
}
BENCHMARK(BM_Llama_RMSNorm_Binary)->Arg(1024)->Arg(2048)->Arg(4096);

static void BM_Llama_SiLU_T81(benchmark::State& state) {
    const int size = state.range(0);
    T729Tensor x({1, size});

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (auto& v : x.data()) v = dis(gen);

    for (auto _ : state) {
        auto res = t81::ops::silu(x);
        benchmark::DoNotOptimize(res);
    }
    state.SetItemsProcessed(state.iterations() * size);
}
BENCHMARK(BM_Llama_SiLU_T81)->Arg(1024)->Arg(4096)->Arg(16384);

static void BM_Llama_SiLU_Binary(benchmark::State& state) {
    const int size = state.range(0);
    std::vector<float> x(size);

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (auto& v : x) v = dis(gen);

    for (auto _ : state) {
        for (int i = 0; i < size; i++) {
            x[i] = x[i] / (1.0f + std::exp(-x[i]));
        }
        benchmark::DoNotOptimize(x);
    }
    state.SetItemsProcessed(state.iterations() * size);
}
BENCHMARK(BM_Llama_SiLU_Binary)->Arg(1024)->Arg(4096)->Arg(16384);

static void BM_Llama_Softmax_T81(benchmark::State& state) {
    const int dim = state.range(0);
    T729Tensor x({1, dim});

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (auto& v : x.data()) v = dis(gen);

    for (auto _ : state) {
        auto res = t81::ops::softmax(x);
        benchmark::DoNotOptimize(res);
    }
    state.SetItemsProcessed(state.iterations() * dim);
}
BENCHMARK(BM_Llama_Softmax_T81)->Arg(1024)->Arg(4096)->Arg(16384);

static void BM_Llama_Softmax_Binary(benchmark::State& state) {
    const int dim = state.range(0);
    std::vector<float> x(dim);

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (auto& v : x) v = dis(gen);

    for (auto _ : state) {
        float max_val = x[0];
        for (int i = 1; i < dim; i++) if (x[i] > max_val) max_val = x[i];
        float sum = 0;
        for (int i = 0; i < dim; i++) {
            x[i] = std::exp(x[i] - max_val);
            sum += x[i];
        }
        float inv_sum = 1.0f / sum;
        for (int i = 0; i < dim; i++) x[i] *= inv_sum;
        benchmark::DoNotOptimize(x);
    }
    state.SetItemsProcessed(state.iterations() * dim);
}
BENCHMARK(BM_Llama_Softmax_Binary)->Arg(1024)->Arg(4096)->Arg(16384);

static void BM_Llama_RoPE_T81(benchmark::State& state) {
    const int head_dim = state.range(0);
    T729Tensor x({1, head_dim});

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (auto& v : x.data()) v = dis(gen);

    for (auto _ : state) {
        auto res = t81::ops::rope(x, 42);
        benchmark::DoNotOptimize(res);
    }
    state.SetItemsProcessed(state.iterations() * head_dim);
}
BENCHMARK(BM_Llama_RoPE_T81)->Arg(128)->Arg(256);

static void BM_Llama_Block_T81(benchmark::State& state) {
    const int hidden_dim = state.range(0);
    T729Tensor x({1, hidden_dim});
    T729Tensor w_norm({hidden_dim});
    T729Tensor w_q({hidden_dim, hidden_dim});

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (auto& v : x.data()) v = dis(gen);
    for (auto& v : w_norm.data()) v = dis(gen);
    for (auto& v : w_q.data()) v = dis(gen);

    for (auto _ : state) {
        auto x_norm = t81::ops::rmsnorm(x, w_norm);
        auto q = t81::ops::matmul(x_norm, w_q);
        auto q_rope = t81::ops::rope(q, 1);
        auto out = t81::ops::softmax(q_rope);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(state.iterations() * hidden_dim);
}
BENCHMARK(BM_Llama_Block_T81)->Arg(1024);
