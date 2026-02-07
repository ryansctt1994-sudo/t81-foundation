#pragma once

#include "t81/vm/state.hpp"
#include "t81/tisc/program.hpp"
#include <vector>
#include <functional>

namespace t81::vm {

/**
 * @class JitTrace
 * @brief A compiled trace of TISC instructions.
 */
class JitTrace {
public:
    virtual ~JitTrace() = default;
    virtual void execute(State& state) = 0;
};

/**
 * @class JitCompiler
 * @brief Minimal trace recorder and compiler for HanoiVM.
 */
class JitCompiler {
public:
    void start_tracing(std::size_t pc);
    void record_instruction(const t81::tisc::Insn& insn);
    std::unique_ptr<JitTrace> compile();

    bool is_tracing() const { return tracing_; }

private:
    bool tracing_{false};
    std::size_t start_pc_{0};
    std::vector<t81::tisc::Insn> trace_buffer_;
};

} // namespace t81::vm
