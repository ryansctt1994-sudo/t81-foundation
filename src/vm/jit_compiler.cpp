#include "t81/vm/jit.hpp"
#include "t81/tensor/llama.hpp"
#include "t81/tensor/matmul.hpp"
#include <iostream>

namespace t81::vm {

class ThreadedJitTrace : public JitTrace {
public:
    explicit ThreadedJitTrace(std::vector<t81::tisc::Insn> insns) : insns_(std::move(insns)) {}

    std::size_t size() const override { return insns_.size(); }

    std::size_t execute(State& state) override {
        auto reg_ok = [&state](int r) {
            return r >= 0 && static_cast<std::size_t>(r) < state.registers.size();
        };
        auto tensor_ptr = [&state](std::int64_t handle) -> t81::T729Tensor* {
            if (handle <= 0) return nullptr;
            const auto idx = static_cast<std::size_t>(handle - 1);
            if (idx >= state.tensors.size()) return nullptr;
            return &state.tensors[idx];
        };
        auto alloc_tensor = [&state](t81::T729Tensor tensor) -> std::int64_t {
            state.tensors.push_back(std::move(tensor));
            return static_cast<std::int64_t>(state.tensors.size());
        };

        std::size_t instructions_executed = 0;
        for (const auto& insn : insns_) {
            instructions_executed++;
            bool stop_trace = false;
            switch (insn.opcode) {
                case t81::tisc::Opcode::Add:
                    state.registers[insn.a] = state.registers[insn.b] + state.registers[insn.c];
                    state.register_tags[insn.a] = ValueTag::Int;
                    break;
                case t81::tisc::Opcode::Sub:
                    state.registers[insn.a] = state.registers[insn.b] - state.registers[insn.c];
                    state.register_tags[insn.a] = ValueTag::Int;
                    break;
                case t81::tisc::Opcode::Mul:
                    state.registers[insn.a] = state.registers[insn.b] * state.registers[insn.c];
                    state.register_tags[insn.a] = ValueTag::Int;
                    break;
                case t81::tisc::Opcode::Div:
                    if (state.registers[insn.c] != 0) {
                        state.registers[insn.a] = state.registers[insn.b] / state.registers[insn.c];
                        state.register_tags[insn.a] = ValueTag::Int;
                    }
                    break;
                case t81::tisc::Opcode::Mod:
                    if (state.registers[insn.c] != 0) {
                        state.registers[insn.a] = state.registers[insn.b] % state.registers[insn.c];
                        state.register_tags[insn.a] = ValueTag::Int;
                    }
                    break;
                case t81::tisc::Opcode::Inc:
                    state.registers[insn.a]++;
                    state.register_tags[insn.a] = ValueTag::Int;
                    break;
                case t81::tisc::Opcode::Dec:
                    state.registers[insn.a]--;
                    state.register_tags[insn.a] = ValueTag::Int;
                    break;
                case t81::tisc::Opcode::Mov:
                    state.registers[insn.a] = state.registers[insn.b];
                    state.register_tags[insn.a] = state.register_tags[insn.b];
                    break;
                case t81::tisc::Opcode::Neg:
                    state.registers[insn.a] = -state.registers[insn.b];
                    state.register_tags[insn.a] = ValueTag::Int;
                    break;
                case t81::tisc::Opcode::LoadImm:
                    state.registers[insn.a] = insn.b;
                    switch (insn.literal_kind) {
                        case t81::tisc::LiteralKind::Int:
                            state.register_tags[insn.a] = ValueTag::Int;
                            break;
                        case t81::tisc::LiteralKind::Bool:
                            state.register_tags[insn.a] = ValueTag::Bool;
                            break;
                        case t81::tisc::LiteralKind::FloatHandle:
                            state.register_tags[insn.a] = ValueTag::FloatHandle;
                            break;
                        case t81::tisc::LiteralKind::FractionHandle:
                            state.register_tags[insn.a] = ValueTag::FractionHandle;
                            break;
                        case t81::tisc::LiteralKind::SymbolHandle:
                            state.register_tags[insn.a] = ValueTag::SymbolHandle;
                            break;
                        case t81::tisc::LiteralKind::TensorHandle:
                            state.register_tags[insn.a] = ValueTag::TensorHandle;
                            break;
                        case t81::tisc::LiteralKind::ShapeHandle:
                            state.register_tags[insn.a] = ValueTag::ShapeHandle;
                            break;
                    }
                    break;
                case t81::tisc::Opcode::TMatMul: {
                    if (!reg_ok(insn.a) || !reg_ok(insn.b) || !reg_ok(insn.c)) {
                        stop_trace = true;
                        break;
                    }
                    if (state.register_tags[insn.b] != ValueTag::TensorHandle ||
                        state.register_tags[insn.c] != ValueTag::TensorHandle) {
                        stop_trace = true;
                        break;
                    }
                    auto ta = tensor_ptr(state.registers[insn.b]);
                    auto tb = tensor_ptr(state.registers[insn.c]);
                    if (!ta || !tb || ta->rank() != 2 || tb->rank() != 2 ||
                        ta->shape()[1] != tb->shape()[0]) {
                        stop_trace = true;
                        break;
                    }
                    state.registers[insn.a] = alloc_tensor(t81::ops::matmul(*ta, *tb));
                    state.register_tags[insn.a] = ValueTag::TensorHandle;
                    break;
                }
                case t81::tisc::Opcode::TRMSNorm: {
                    if (!reg_ok(insn.a) || !reg_ok(insn.b) || !reg_ok(insn.c)) {
                        stop_trace = true;
                        break;
                    }
                    if (state.register_tags[insn.b] != ValueTag::TensorHandle ||
                        state.register_tags[insn.c] != ValueTag::TensorHandle) {
                        stop_trace = true;
                        break;
                    }
                    auto t = tensor_ptr(state.registers[insn.b]);
                    auto w = tensor_ptr(state.registers[insn.c]);
                    if (!t || !w || t->rank() == 0 || w->rank() != 1 || w->shape()[0] != t->shape().back()) {
                        stop_trace = true;
                        break;
                    }
                    state.registers[insn.a] = alloc_tensor(t81::ops::rmsnorm(*t, *w));
                    state.register_tags[insn.a] = ValueTag::TensorHandle;
                    break;
                }
                case t81::tisc::Opcode::Less:
                    state.registers[insn.a] = (state.registers[insn.b] < state.registers[insn.c]) ? 1 : 0;
                    state.register_tags[insn.a] = ValueTag::Int;
                    break;
                case t81::tisc::Opcode::Equal:
                    state.registers[insn.a] = (state.registers[insn.b] == state.registers[insn.c]) ? 1 : 0;
                    state.register_tags[insn.a] = ValueTag::Int;
                    break;
                case t81::tisc::Opcode::Jump:
                    state.pc = static_cast<size_t>(insn.a);
                    stop_trace = true;
                    break;
                case t81::tisc::Opcode::JumpIfZero:
                    if (state.registers[insn.b] == 0) {
                        state.pc = static_cast<size_t>(insn.a);
                        stop_trace = true;
                    }
                    break;
                case t81::tisc::Opcode::JumpIfNotZero:
                    if (state.registers[insn.b] != 0) {
                        state.pc = static_cast<size_t>(insn.a);
                        stop_trace = true;
                    }
                    break;
                default:
                    break;
            }
            if (insn.opcode != t81::tisc::Opcode::Jump &&
                insn.opcode != t81::tisc::Opcode::JumpIfZero &&
                insn.opcode != t81::tisc::Opcode::JumpIfNotZero) {
                state.flags.zero = (state.registers[insn.a] == 0);
                state.flags.negative = (state.registers[insn.a] < 0);
                state.flags.positive = (state.registers[insn.a] > 0);
            }
            if (stop_trace) return instructions_executed;
        }
        state.pc += instructions_executed;
        return instructions_executed;
    }

private:
    std::vector<t81::tisc::Insn> insns_;
};

void JitCompiler::start_tracing(std::size_t pc) {
    tracing_ = true;
    start_pc_ = pc;
    trace_buffer_.clear();
}

void JitCompiler::record_instruction(const t81::tisc::Insn& insn) {
    if (!tracing_) return;

    // Only record supported opcodes
    switch (insn.opcode) {
        case t81::tisc::Opcode::Add:
        case t81::tisc::Opcode::Sub:
        case t81::tisc::Opcode::Mul:
        case t81::tisc::Opcode::Div:
        case t81::tisc::Opcode::Mod:
        case t81::tisc::Opcode::Inc:
        case t81::tisc::Opcode::Dec:
        case t81::tisc::Opcode::Mov:
        case t81::tisc::Opcode::Neg:
        case t81::tisc::Opcode::LoadImm:
        case t81::tisc::Opcode::Less:
        case t81::tisc::Opcode::Equal:
        case t81::tisc::Opcode::TMatMul:
        case t81::tisc::Opcode::TRMSNorm:
            trace_buffer_.push_back(insn);
            break;
        case t81::tisc::Opcode::Jump:
        case t81::tisc::Opcode::JumpIfZero:
        case t81::tisc::Opcode::JumpIfNotZero:
            trace_buffer_.push_back(insn);
            tracing_ = false; // Always stop at jump
            break;
        default:
            // Stop tracing on unsupported opcode
            tracing_ = false;
            break;
    }
}

std::unique_ptr<JitTrace> JitCompiler::compile() {
    tracing_ = false;
    if (trace_buffer_.empty()) return nullptr;
    return std::make_unique<ThreadedJitTrace>(std::move(trace_buffer_));
}

} // namespace t81::vm
