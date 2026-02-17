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

static void BM_TensorMatMul_Naive_Binary(benchmark::State& state) {
    const int M = state.range(0);
    const int K = state.range(0);
    const int N = state.range(0);

    std::vector<float> A(static_cast<std::size_t>(M) * K);
    std::vector<float> B(static_cast<std::size_t>(K) * N);
    std::vector<float> C(static_cast<std::size_t>(M) * N, 0.0f);

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (auto& v : A) v = dis(gen);
    for (auto& v : B) v = dis(gen);

    for (auto _ : state) {
        std::fill(C.begin(), C.end(), 0.0f);
        for (int i = 0; i < M; ++i) {
            for (int k = 0; k < K; ++k) {
                const float a = A[static_cast<std::size_t>(i) * K + k];
                for (int j = 0; j < N; ++j) {
                    C[static_cast<std::size_t>(i) * N + j] +=
                        a * B[static_cast<std::size_t>(k) * N + j];
                }
            }
        }
        benchmark::DoNotOptimize(C);
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(M) * N * K * 2);
}

BENCHMARK(BM_TensorMatMul_Naive_Binary)->Arg(64)->Arg(128)->Arg(256)->Arg(512);
