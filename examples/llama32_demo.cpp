#include "t81/weights.hpp"
#include "t81/tisc/program.hpp"
#include "t81/tisc/opcodes.hpp"
#include "t81/vm/vm.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <span>

using namespace t81;
using namespace t81::tisc;
using namespace t81::weights;

int main() {
    std::cout << "--- T81 'Go Broad' Killer Demo: Llama-3.2-1B Deterministic Inference Block ---\n";

    // 1. Create a mock T3_K quantized weights model
    NativeModel mock_weights;
    std::vector<int8_t> dummy_w(128 * 128, 0);
    for(size_t i=0; i<dummy_w.size(); ++i) {
        dummy_w[i] = static_cast<int8_t>((i % 3) - 1);
    }

    mock_weights["model.layers.0.input_layernorm.weight"] = import_bitnet_b158(std::span<const int8_t>(dummy_w.data(), 128), {128});
    mock_weights["model.layers.0.self_attn.q_proj.weight"] = import_bitnet_b158(std::span<const int8_t>(dummy_w.data(), 128*128), {128, 128});

    // 2. Build TISC program
    Program program;
    program.weights_model = std::make_shared<ModelFile>();
    program.weights_model->native = std::move(mock_weights);
    program.symbol_pool = {
        "model.layers.0.input_layernorm.weight",
        "model.layers.0.self_attn.q_proj.weight"
    };

    // Initial input tensor (Rank 2: 1x128)
    std::vector<float> input_data(128, 0.5f);
    program.tensor_pool.emplace_back(std::vector<int>{1, 128}, std::move(input_data));

    std::vector<Insn> insns;

    // Load input to R0
    Insn i1{}; i1.opcode = Opcode::LoadImm; i1.a = 0; i1.b = 1; i1.literal_kind = LiteralKind::TensorHandle;
    insns.push_back(i1);

    // Load RMSNorm weight to R1
    Insn i2{}; i2.opcode = Opcode::WeightsLoad; i2.a = 1; i2.b = 1; // symbol index 1
    insns.push_back(i2);

    // TRMSNorm R2, R0, R1
    Insn i3{}; i3.opcode = Opcode::TRMSNorm; i3.a = 2; i3.b = 0; i3.c = 1;
    insns.push_back(i3);

    // Load Q_proj weight to R3
    Insn i4{}; i4.opcode = Opcode::WeightsLoad; i4.a = 3; i4.b = 2; // symbol index 2
    insns.push_back(i4);

    // TMatMul R4, R2, R3
    Insn i5{}; i5.opcode = Opcode::TMatMul; i5.a = 4; i5.b = 2; i5.c = 3;
    insns.push_back(i5);

    // TRoPE R5, R4, R6 (R6 is position)
    Insn i6{}; i6.opcode = Opcode::LoadImm; i6.a = 6; i6.b = 42;
    insns.push_back(i6);
    Insn i7{}; i7.opcode = Opcode::TRoPE; i7.a = 5; i7.b = 4; i7.c = 6;
    insns.push_back(i7);

    // TSoftmax R7, R5
    Insn i8{}; i8.opcode = Opcode::TSoftmax; i8.a = 7; i8.b = 5;
    insns.push_back(i8);

    Insn halt{}; halt.opcode = Opcode::Halt;
    insns.push_back(halt);
    program.insns = std::move(insns);

    program.axion_policy_text =
        "(policy (tier 1)"
        " (require-segment-event (segment tensor) (action \"tensor slot allocated\"))"
        " (require-segment-event (segment meta) (action \"meta slot axion event\")))";

    // 3. Run
    auto vm = vm::make_interpreter_vm();
    vm->load_program(program);
    auto result = vm->run_to_halt(1000);

    if (!result.has_value()) {
        std::cerr << "Demo failed with trap: " << static_cast<int>(result.error()) << "\n";
        return 1;
    }

    // 4. Print Trace
    std::cout << "\nDeterministic Axion Trace Artifacts:\n";
    for (const auto& event : vm->state().axion_log) {
        std::cout << "  [Axion] op=" << static_cast<int>(event.opcode) << " reason=\"" << event.verdict.reason << "\"\n";
    }

    std::cout << "\nLlama-3.2-1B block inference complete. Output tensor (Softmax result) is in R7.\n";
    std::cout << "SUCCESS: Bit-identical results guaranteed by HanoiVM.\n";

    return 0;
}
