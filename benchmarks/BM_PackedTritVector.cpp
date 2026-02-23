#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include <algorithm>
#include "t81/experimental/packed_trit_vector.hpp"

using namespace t81::experimental;

static void BM_ScalarTAnd(benchmark::State& state) {
    size_t len = state.range(0);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-1, 1);
    std::vector<int8_t> v1(len), v2(len), res(len);
    for (size_t i = 0; i < len; ++i) {
        v1[i] = static_cast<int8_t>(dist(rng));
        v2[i] = static_cast<int8_t>(dist(rng));
    }

    for (auto _ : state) {
        for (size_t i = 0; i < len; ++i) {
            res[i] = PackedTritVector::scalar_and(v1[i], v2[i]);
        }
        benchmark::DoNotOptimize(res.data());
    }
}
BENCHMARK(BM_ScalarTAnd)->Range(16, 4096);

static void BM_PackedTAnd(benchmark::State& state) {
    size_t len = state.range(0);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-1, 1);
    std::vector<int8_t> t1(len), t2(len);
    for (size_t i = 0; i < len; ++i) {
        t1[i] = static_cast<int8_t>(dist(rng));
        t2[i] = static_cast<int8_t>(dist(rng));
    }

    auto p1 = PackedTritVector::from_trits(t1).value();
    auto p2 = PackedTritVector::from_trits(t2).value();

    for (auto _ : state) {
        auto res = p1.t_and(p2).value();
        benchmark::DoNotOptimize(res.packed_data().data());
    }
}
BENCHMARK(BM_PackedTAnd)->Range(16, 4096);

static void BM_ScalarTOr(benchmark::State& state) {
    size_t len = state.range(0);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-1, 1);
    std::vector<int8_t> v1(len), v2(len), res(len);
    for (size_t i = 0; i < len; ++i) {
        v1[i] = static_cast<int8_t>(dist(rng));
        v2[i] = static_cast<int8_t>(dist(rng));
    }

    for (auto _ : state) {
        for (size_t i = 0; i < len; ++i) {
            res[i] = PackedTritVector::scalar_or(v1[i], v2[i]);
        }
        benchmark::DoNotOptimize(res.data());
    }
}
BENCHMARK(BM_ScalarTOr)->Range(16, 4096);

static void BM_PackedTOr(benchmark::State& state) {
    size_t len = state.range(0);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-1, 1);
    std::vector<int8_t> t1(len), t2(len);
    for (size_t i = 0; i < len; ++i) {
        t1[i] = static_cast<int8_t>(dist(rng));
        t2[i] = static_cast<int8_t>(dist(rng));
    }

    auto p1 = PackedTritVector::from_trits(t1).value();
    auto p2 = PackedTritVector::from_trits(t2).value();

    for (auto _ : state) {
        auto res = p1.t_or(p2).value();
        benchmark::DoNotOptimize(res.packed_data().data());
    }
}
BENCHMARK(BM_PackedTOr)->Range(16, 4096);

static void BM_ScalarTXor(benchmark::State& state) {
    size_t len = state.range(0);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-1, 1);
    std::vector<int8_t> v1(len), v2(len), res(len);
    for (size_t i = 0; i < len; ++i) {
        v1[i] = static_cast<int8_t>(dist(rng));
        v2[i] = static_cast<int8_t>(dist(rng));
    }

    for (auto _ : state) {
        for (size_t i = 0; i < len; ++i) {
            res[i] = PackedTritVector::scalar_xor(v1[i], v2[i]);
        }
        benchmark::DoNotOptimize(res.data());
    }
}
BENCHMARK(BM_ScalarTXor)->Range(16, 4096);

static void BM_PackedTXor(benchmark::State& state) {
    size_t len = state.range(0);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-1, 1);
    std::vector<int8_t> t1(len), t2(len);
    for (size_t i = 0; i < len; ++i) {
        t1[i] = static_cast<int8_t>(dist(rng));
        t2[i] = static_cast<int8_t>(dist(rng));
    }

    auto p1 = PackedTritVector::from_trits(t1).value();
    auto p2 = PackedTritVector::from_trits(t2).value();

    for (auto _ : state) {
        auto res = p1.t_xor(p2).value();
        benchmark::DoNotOptimize(res.packed_data().data());
    }
}
BENCHMARK(BM_PackedTXor)->Range(16, 4096);

static void BM_ScalarTNot(benchmark::State& state) {
    size_t len = state.range(0);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-1, 1);
    std::vector<int8_t> v1(len), res(len);
    for (size_t i = 0; i < len; ++i) {
        v1[i] = static_cast<int8_t>(dist(rng));
    }

    for (auto _ : state) {
        for (size_t i = 0; i < len; ++i) {
            res[i] = PackedTritVector::scalar_not(v1[i]);
        }
        benchmark::DoNotOptimize(res.data());
    }
}
BENCHMARK(BM_ScalarTNot)->Range(16, 4096);

static void BM_PackedTNot(benchmark::State& state) {
    size_t len = state.range(0);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-1, 1);
    std::vector<int8_t> t1(len);
    for (size_t i = 0; i < len; ++i) {
        t1[i] = static_cast<int8_t>(dist(rng));
    }

    auto p1 = PackedTritVector::from_trits(t1).value();

    for (auto _ : state) {
        auto res = p1.t_not().value();
        benchmark::DoNotOptimize(res.packed_data().data());
    }
}
BENCHMARK(BM_PackedTNot)->Range(16, 4096);

static void BM_PackPT5(benchmark::State& state) {
    size_t len = state.range(0);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-1, 1);
    std::vector<int8_t> t1(len);
    for (size_t i = 0; i < len; ++i) {
        t1[i] = static_cast<int8_t>(dist(rng));
    }

    for (auto _ : state) {
        auto res = PackedTritVector::from_trits(t1).value();
        benchmark::DoNotOptimize(res.packed_data().data());
    }
}
BENCHMARK(BM_PackPT5)->Range(16, 4096);

static void BM_UnpackPT5(benchmark::State& state) {
    size_t len = state.range(0);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-1, 1);
    std::vector<int8_t> t1(len);
    for (size_t i = 0; i < len; ++i) {
        t1[i] = static_cast<int8_t>(dist(rng));
    }
    auto p1 = PackedTritVector::from_trits(t1).value();

    for (auto _ : state) {
        auto res = p1.to_trits().value();
        benchmark::DoNotOptimize(res.data());
    }
}
BENCHMARK(BM_UnpackPT5)->Range(16, 4096);
