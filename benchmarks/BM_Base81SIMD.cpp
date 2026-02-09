#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include "t81/simd/base81_digits.hpp"

using namespace t81::simd::base81_digits;

static void BM_Base81_Add_Binary(benchmark::State& state) {
    size_t n = state.range(0);
    std::vector<uint8_t> a(n, 40);
    std::vector<uint8_t> b(n, 41);
    std::vector<uint8_t> out(n);

    for (auto _ : state) {
        int carry = 0;
        for (size_t i = 0; i < n; ++i) {
            int val = static_cast<int>(a[i]) + static_cast<int>(b[i]) - 40 + carry;
            if (val > 80) { carry = 1; val -= 81; }
            else if (val < 0) { carry = -1; val += 81; }
            else { carry = 0; }
            out[i] = static_cast<uint8_t>(val);
        }
        benchmark::DoNotOptimize(out.data());
    }
    state.SetBytesProcessed(state.iterations() * n);
}
BENCHMARK(BM_Base81_Add_Binary)->Arg(1024)->Arg(65536);

static void BM_Base81_Add_T81(benchmark::State& state) {
    size_t n = state.range(0);
    std::vector<uint8_t> a(n, 40);
    std::vector<uint8_t> b(n, 41);
    std::vector<uint8_t> raw(n);
    std::vector<uint8_t> out(n);

    for (auto _ : state) {
        add(a, b, raw);
        normalize_add(raw, out);
        benchmark::DoNotOptimize(out.data());
    }
    state.SetBytesProcessed(state.iterations() * n);
}
BENCHMARK(BM_Base81_Add_T81)->Arg(1024)->Arg(65536);

static void BM_Base81_Negate_Binary(benchmark::State& state) {
    size_t n = state.range(0);
    std::vector<uint8_t> in(n, 41);
    std::vector<uint8_t> out(n);
    for (auto _ : state) {
        for (size_t i = 0; i < n; ++i) {
            out[i] = 80 - in[i];
        }
        benchmark::DoNotOptimize(out.data());
    }
    state.SetBytesProcessed(state.iterations() * n);
}
BENCHMARK(BM_Base81_Negate_Binary)->Arg(1024)->Arg(65536);

static void BM_Base81_Negate_T81(benchmark::State& state) {
    size_t n = state.range(0);
    std::vector<uint8_t> in(n, 41);
    std::vector<uint8_t> out(n);
    for (auto _ : state) {
        negate(in, out);
        benchmark::DoNotOptimize(out.data());
    }
    state.SetBytesProcessed(state.iterations() * n);
}
BENCHMARK(BM_Base81_Negate_T81)->Arg(1024)->Arg(65536);
