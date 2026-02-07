#include <benchmark/benchmark.h>
#include "t81/tensor.hpp"
#include "t81/tensor/matmul.hpp"
#include <vector>
#include <random>

using namespace t81;

static void BM_TensorMatMul_Naive(benchmark::State& state) {
    const int M = state.range(0);
    const int K = state.range(0);
    const int N = state.range(0);

    T729Tensor A({M, K});
    T729Tensor B({K, N});

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (auto& v : A.data()) v = dis(gen);
    for (auto& v : B.data()) v = dis(gen);

    for (auto _ : state) {
        auto C = t81::ops::matmul(A, B);
        benchmark::DoNotOptimize(C);
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(M) * N * K * 2);
}

BENCHMARK(BM_TensorMatMul_Naive)->Arg(64)->Arg(128)->Arg(256)->Arg(512);
