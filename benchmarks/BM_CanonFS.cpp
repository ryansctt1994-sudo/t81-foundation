#include <benchmark/benchmark.h>
#include "t81/canonfs/canon_driver.hpp"
#include <vector>
#include <cstddef>
#include <filesystem>
#include <random>

using namespace t81::canonfs;

static void BM_CanonFS_WriteThroughput_InMemory(benchmark::State& state) {
    auto driver = make_in_memory_driver();
    size_t data_size = state.range(0);
    std::vector<std::byte> data(data_size, std::byte{0x42});

    for (auto _ : state) {
        auto res = driver->write_object(ObjectType::Blob, data);
        benchmark::DoNotOptimize(res);
    }
    state.SetBytesProcessed(state.iterations() * data_size);
}
BENCHMARK(BM_CanonFS_WriteThroughput_InMemory)->Range(1024, 1024 * 1024);

static void BM_CanonFS_ReadThroughput_InMemory(benchmark::State& state) {
    auto driver = make_in_memory_driver();
    size_t data_size = state.range(0);
    std::vector<std::byte> data(data_size, std::byte{0x42});
    auto ref_res = driver->write_object(ObjectType::Blob, data);
    auto ref = ref_res.value();

    for (auto _ : state) {
        auto res = driver->read_object_bytes(ref);
        benchmark::DoNotOptimize(res);
    }
    state.SetBytesProcessed(state.iterations() * data_size);
}
BENCHMARK(BM_CanonFS_ReadThroughput_InMemory)->Range(1024, 1024 * 1024);

static void BM_CanonFS_WriteThroughput_Persistent(benchmark::State& state) {
    std::filesystem::path root = "bench_canonfs_root";
    std::filesystem::remove_all(root);
    auto driver = make_persistent_driver(root);
    size_t data_size = state.range(0);
    std::vector<std::byte> data(data_size, std::byte{0x42});

    // We need different data each time to avoid content-addressing bypass if any,
    // though the current persistent driver always writes.
    // Actually, let's use a counter to make it different.
    uint64_t counter = 0;

    for (auto _ : state) {
        std::memcpy(data.data(), &counter, std::min(sizeof(counter), data_size));
        auto res = driver->write_object(ObjectType::Blob, data);
        benchmark::DoNotOptimize(res);
        counter++;
    }
    state.SetBytesProcessed(state.iterations() * data_size);
    std::filesystem::remove_all(root);
}
BENCHMARK(BM_CanonFS_WriteThroughput_Persistent)->Range(1024, 1024 * 1024);

static void BM_CanonFS_ReadThroughput_Persistent(benchmark::State& state) {
    std::filesystem::path root = "bench_canonfs_read_root";
    std::filesystem::remove_all(root);
    auto driver = make_persistent_driver(root);
    size_t data_size = state.range(0);
    std::vector<std::byte> data(data_size, std::byte{0x42});
    auto ref_res = driver->write_object(ObjectType::Blob, data);
    auto ref = ref_res.value();

    for (auto _ : state) {
        auto res = driver->read_object_bytes(ref);
        benchmark::DoNotOptimize(res);
    }
    state.SetBytesProcessed(state.iterations() * data_size);
    std::filesystem::remove_all(root);
}
BENCHMARK(BM_CanonFS_ReadThroughput_Persistent)->Range(1024, 1024 * 1024);
