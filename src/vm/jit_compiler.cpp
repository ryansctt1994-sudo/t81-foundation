#include "t81/vm/jit.hpp"
#include <iostream>

namespace t81::vm {

class ThreadedJitTrace : public JitTrace {
public:
    explicit ThreadedJitTrace(std::vector<t81::tisc::Insn> insns) : insns_(std::move(insns)) {}

    void execute(State& state) override {
        for (const auto& insn : insns_) {
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
                    state.register_tags[insn.a] = (insn.literal_kind == t81::tisc::LiteralKind::Int) ? ValueTag::Int : ValueTag::SymbolHandle;
                    break;
                case t81::tisc::Opcode::Less:
                    state.registers[insn.a] = (state.registers[insn.b] < state.registers[insn.c]) ? 1 : 0;
                    state.register_tags[insn.a] = ValueTag::Int;
                    break;
                case t81::tisc::Opcode::Equal:
                    state.registers[insn.a] = (state.registers[insn.b] == state.registers[insn.c]) ? 1 : 0;
                    state.register_tags[insn.a] = ValueTag::Int;
                    break;
                default:
                    break;
            }
            state.flags.zero = (state.registers[insn.a] == 0);
            state.flags.negative = (state.registers[insn.a] < 0);
            state.flags.positive = (state.registers[insn.a] > 0);
        }
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
            trace_buffer_.push_back(insn);
            break;
        default:
            // Stop tracing on unsupported opcode or branch
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
