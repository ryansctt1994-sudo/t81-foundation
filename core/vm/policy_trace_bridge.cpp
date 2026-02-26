#include "internal/policy_trace_bridge.hpp"

#include <algorithm>
#include <sstream>

#include "t81/axion/reasons.hpp"

namespace t81::vm::internal {

t81::axion::SyscallContext make_syscall_context(const State& state, std::size_t current_context,
                                                std::string_view caller, std::string_view syscall,
                                                std::string_view payload, std::size_t pc,
                                                t81::tisc::Opcode opcode,
                                                std::size_t instruction_count,
                                                std::optional<std::size_t> instruction_override) {
  t81::axion::SyscallContext sys_ctx;
  sys_ctx.caller.assign(caller);
  sys_ctx.syscall.assign(syscall);
  sys_ctx.payload.assign(payload);
  sys_ctx.pc = pc;
  sys_ctx.next_opcode = opcode;
  sys_ctx.instruction_count = instruction_override.value_or(instruction_count);

  if (!state.contexts.empty() && current_context < state.contexts.size()) {
    const auto& tctx = state.contexts[current_context];
    sys_ctx.recursion_depth = std::max(tctx.stack_frames.size(), tctx.call_depth);
    sys_ctx.stack_usage = tctx.stack_base - tctx.sp;
    sys_ctx.current_tier = static_cast<int>(tctx.tier_status.current);
  } else {
    sys_ctx.recursion_depth = 0;
    sys_ctx.stack_usage = 0;
    sys_ctx.current_tier = 0;
  }

  sys_ctx.reflection_count = state.reflection_count;
  sys_ctx.meta_write_count = state.meta_write_count;
  sys_ctx.policy = state.policy ? &*state.policy : nullptr;
  sys_ctx.trace_reasons.reserve(state.axion_log.size());
  for (const auto& entry : state.axion_log) {
    sys_ctx.trace_reasons.push_back(entry.verdict.reason);
  }
  return sys_ctx;
}

std::string format_memory_access_reason(MemorySegmentKind kind, std::size_t addr, std::size_t size,
                                        std::string_view action) {
  std::ostringstream reason;
  reason << action << " " << to_string(kind) << " addr=" << addr;
  if (kind == MemorySegmentKind::Stack || action.find("allocated") != std::string_view::npos ||
      action.find("freed") != std::string_view::npos) {
    reason << " size=" << size;
  } else if (size > 1) {
    reason << " size=" << size;
  }
  return reason.str();
}

std::string format_bounds_fault_reason(MemorySegmentKind kind, int addr, std::string_view action) {
  std::ostringstream reason;
  reason << t81::axion::reasons::kBoundsFault << " segment=" << to_string(kind) << " addr=" << addr
         << " action=" << action;
  return reason.str();
}

std::string append_segment_reason(std::string_view action, MemorySegmentKind kind, std::size_t addr,
                                  std::string_view base_reason) {
  std::ostringstream reason;
  reason << action << " segment=" << to_string(kind) << " addr=" << addr;
  if (!base_reason.empty()) {
    reason << " " << base_reason;
  }
  return reason.str();
}

}  // namespace t81::vm::internal
