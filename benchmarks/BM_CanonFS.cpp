#include <benchmark/benchmark.h>
#include "t81/canonfs/canon_driver.hpp"
#include <vector>
#include <cstddef>
#include <filesystem>
#include <random>
#include <fstream>

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
BENCHMARK(BM_CanonFS_WriteThroughput_InMemory)
    ->Name("BM_CanonFS_WriteThroughput_InMemory_T81")
    ->Range(1024, 1024 * 1024);

static void BM_CanonFS_WriteThroughput_Binary(benchmark::State& state) {
    size_t data_size = state.range(0);
    std::vector<std::byte> data(data_size, std::byte{0x42});
    std::vector<std::byte> target;
    for (auto _ : state) {
        target = data;
        benchmark::DoNotOptimize(target);
    }
    state.SetBytesProcessed(state.iterations() * data_size);
}
BENCHMARK(BM_CanonFS_WriteThroughput_Binary)
    ->Name("BM_CanonFS_WriteThroughput_InMemory_Binary")
    ->Range(1024, 1024 * 1024);

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
BENCHMARK(BM_CanonFS_ReadThroughput_InMemory)
    ->Name("BM_CanonFS_ReadThroughput_InMemory_T81")
    ->Range(1024, 1024 * 1024);

static void BM_CanonFS_ReadThroughput_Binary(benchmark::State& state) {
    size_t data_size = state.range(0);
    std::vector<std::byte> data(data_size, std::byte{0x42});
    for (auto _ : state) {
        std::vector<std::byte> copy = data;
        benchmark::DoNotOptimize(copy);
    }
    state.SetBytesProcessed(state.iterations() * data_size);
}
BENCHMARK(BM_CanonFS_ReadThroughput_Binary)
    ->Name("BM_CanonFS_ReadThroughput_InMemory_Binary")
    ->Range(1024, 1024 * 1024);

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
BENCHMARK(BM_CanonFS_WriteThroughput_Persistent)
    ->Name("BM_CanonFS_WriteThroughput_Persistent_T81")
    ->Range(1024, 1024 * 1024);

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
BENCHMARK(BM_CanonFS_ReadThroughput_Persistent)
    ->Name("BM_CanonFS_ReadThroughput_Persistent_T81")
    ->Range(1024, 1024 * 1024);

static void BM_CanonFS_WriteThroughput_Persistent_Binary(benchmark::State& state) {
    size_t data_size = state.range(0);
    std::vector<std::byte> data(data_size, std::byte{0x42});
    std::filesystem::path p = "bench_binary_persistent.tmp";
    for (auto _ : state) {
        std::ofstream ofs(p, std::ios::binary);
        ofs.write(reinterpret_cast<const char*>(data.data()), data_size);
        benchmark::DoNotOptimize(ofs);
    }
    state.SetBytesProcessed(state.iterations() * data_size);
    std::filesystem::remove(p);
}
BENCHMARK(BM_CanonFS_WriteThroughput_Persistent_Binary)
    ->Name("BM_CanonFS_WriteThroughput_Persistent_Binary")
    ->Range(1024, 1024 * 1024);

static void BM_CanonFS_ReadThroughput_Persistent_Binary(benchmark::State& state) {
    size_t data_size = state.range(0);
    std::vector<std::byte> data(data_size, std::byte{0x42});
    std::filesystem::path p = "bench_binary_persistent_read.tmp";
    {
        std::ofstream ofs(p, std::ios::binary);
        ofs.write(reinterpret_cast<const char*>(data.data()), data_size);
    }
    for (auto _ : state) {
        std::ifstream ifs(p, std::ios::binary);
        ifs.read(reinterpret_cast<char*>(data.data()), data_size);
        benchmark::DoNotOptimize(ifs);
    }
    state.SetBytesProcessed(state.iterations() * data_size);
    std::filesystem::remove(p);
}
BENCHMARK(BM_CanonFS_ReadThroughput_Persistent_Binary)
    ->Name("BM_CanonFS_ReadThroughput_Persistent_Binary")
    ->Range(1024, 1024 * 1024);
