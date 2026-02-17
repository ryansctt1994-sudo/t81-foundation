#include <benchmark/benchmark.h>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <cstdint>

#include "t81/vm/vm.hpp"
#include "t81/tisc/program.hpp"
#include "t81/weights.hpp"
#include "t81/tisc/opcodes.hpp"

// This benchmark measures the time taken to promote a tensor from native weights.
// The promotion logic involves copying the shape vector, which is the target of our optimization.
template <int Rank>
static void BM_TensorPromotion(benchmark::State& state) {
    t81::tisc::Program program;
    program.symbol_pool = {"tensorA"};

    // Create a native tensor with a large shape but minimal data
    auto model = std::make_shared<t81::weights::ModelFile>();
    t81::weights::NativeTensor tensor;
    tensor.trits = 1; // Minimal data
    tensor.data = {0}; // Minimal data

    // Populate shape with dummy dimensions
    for (int i = 0; i < Rank; ++i) {
        tensor.shape.push_back(1);
    }

    model->native["tensorA"] = tensor;
    program.weights_model = model;

    t81::tisc::Insn load_w;
    load_w.opcode = t81::tisc::Opcode::WeightsLoad;
    load_w.a = 1; // R1
    load_w.b = 1; // Symbol index 1 ("tensorA")

    t81::tisc::Insn texp;
    texp.opcode = t81::tisc::Opcode::TExp;
    texp.a = 2; // R2
    texp.b = 1; // R1

    t81::tisc::Insn jump;
    jump.opcode = t81::tisc::Opcode::Jump;
    jump.a = 0; // Jump to PC=0

    program.insns = {load_w, texp, jump};

    auto vm = t81::vm::make_interpreter_vm();
    vm->load_program(program);

    for (auto _ : state) {
        // Execute one iteration of the loop (3 instructions)
        auto res1 = vm->step();
        if (!res1) {
            state.SkipWithError("WeightsLoad failed");
            break;
        }

        auto res2 = vm->step();
        if (!res2) {
            state.SkipWithError("TExp failed");
            break;
        }

        auto res3 = vm->step();
        if (!res3) {
            state.SkipWithError("Jump failed");
            break;
        }
    }
}

// Register the benchmark with a high rank (1,000,000) to exaggerate the allocation cost.
BENCHMARK_TEMPLATE(BM_TensorPromotion, 1000000)
    ->Name("BM_TensorPromotion_T81");

template <int Rank>
static void BM_TensorPromotion_Binary(benchmark::State& state) {
    std::vector<std::uint32_t> shape(static_cast<std::size_t>(Rank), 1u);
    std::vector<std::uint32_t> promoted;
    promoted.reserve(shape.size());

    for (auto _ : state) {
        promoted.assign(shape.begin(), shape.end());
        benchmark::DoNotOptimize(promoted.data());
    }
    state.SetItemsProcessed(state.iterations() * Rank);
}

BENCHMARK_TEMPLATE(BM_TensorPromotion_Binary, 1000000)
    ->Name("BM_TensorPromotion_Binary");
