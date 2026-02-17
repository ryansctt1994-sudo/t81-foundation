#include <benchmark/benchmark.h>
#include "t81/core/T81Limb.hpp"
#include <cstdint>

using namespace t81::core;

static void BM_LimbMul_Booth(benchmark::State& state) {
    T81Limb a, b;
    for (int i = 0; i < T81Limb::TRYTES; ++i) {
        a.set_tryte(i, (i % 27) - 13);
        b.set_tryte(i, ((i + 7) % 27) - 13);
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(a * b);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LimbMul_Booth)->Unit(benchmark::kMillisecond);

static void BM_LimbMul_Booth_Binary(benchmark::State& state) {
    std::uint64_t a = 0x123456789abcdef0ULL;
    std::uint64_t b = 0x0fedcba987654321ULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(static_cast<unsigned __int128>(a) * b);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LimbMul_Booth_Binary)->Unit(benchmark::kMillisecond);

static void BM_LimbMul_Booth_Real(benchmark::State& state) {
    T81Limb a, b;
    for (int i = 0; i < T81Limb::TRYTES; ++i) {
        a.set_tryte(i, (i % 26) - 13);
        b.set_tryte(i, ((i + 5) % 26) - 13);
    }

    for (auto _ : state) {
        T81Limb result = T81Limb::booth_mul(a, b);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LimbMul_Booth_Real)->Unit(benchmark::kMillisecond);

static void BM_LimbMul_Booth_Real_Binary(benchmark::State& state) {
    std::uint64_t a = 0x9abcdef012345678ULL;
    std::uint64_t b = 0x13579bdf2468ace0ULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(static_cast<unsigned __int128>(a) * b);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LimbMul_Booth_Real_Binary)->Unit(benchmark::kMillisecond);

static void BM_LimbAdd_KoggeStone(benchmark::State& state) {
    T81Limb a, b;
    for (int i = 0; i < T81Limb::TRYTES; ++i) {
        a.set_tryte(i, (i % 27) - 13);
        b.set_tryte(i, ((i + 5) % 27) - 13);
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(a + b);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LimbAdd_KoggeStone)->Unit(benchmark::kMillisecond);

static void BM_LimbAdd_KoggeStone_Binary(benchmark::State& state) {
    std::uint64_t a = 0x123456789abcdef0ULL;
    std::uint64_t b = 0x0fedcba987654321ULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(a + b);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LimbAdd_KoggeStone_Binary)->Unit(benchmark::kMillisecond);

static void BM_Limb54Mul_Booth(benchmark::State& state) {
    T81Limb54 a, b;
    for (int i = 0; i < T81Limb54::TRYTES; ++i) {
        a.set_tryte(i, (i % 27) - 13);
        b.set_tryte(i, ((i + 7) % 27) - 13);
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(a * b);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Limb54Mul_Booth)->Unit(benchmark::kMillisecond);

static void BM_Limb54Mul_Booth_Binary(benchmark::State& state) {
    std::uint64_t a = 0x123456789abcdef0ULL;
    std::uint64_t b = 0x0fedcba987654321ULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(static_cast<unsigned __int128>(a) * b);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Limb54Mul_Booth_Binary)->Unit(benchmark::kMillisecond);

static void BM_Limb54Add_KoggeStone(benchmark::State& state) {
    T81Limb54 a, b;
    for (int i = 0; i < T81Limb54::TRYTES; ++i) {
        a.set_tryte(i, (i % 27) - 13);
        b.set_tryte(i, ((i + 5) % 27) - 13);
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(a + b);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Limb54Add_KoggeStone)->Unit(benchmark::kMillisecond);

static void BM_Limb54Add_KoggeStone_Binary(benchmark::State& state) {
    std::uint64_t a = 0x123456789abcdef0ULL;
    std::uint64_t b = 0x0fedcba987654321ULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(a + b);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Limb54Add_KoggeStone_Binary)->Unit(benchmark::kMillisecond);
