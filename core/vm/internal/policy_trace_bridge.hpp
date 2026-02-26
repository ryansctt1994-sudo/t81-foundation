#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

#include "t81/axion/context.hpp"
#include "t81/isa/opcodes.hpp"
#include "t81/vm/state.hpp"

namespace t81::vm::internal {

t81::axion::SyscallContext make_syscall_context(const State& state, std::size_t current_context,
                                                std::string_view caller, std::string_view syscall,
                                                std::string_view payload, std::size_t pc,
                                                t81::tisc::Opcode opcode,
                                                std::size_t instruction_count,
                                                std::optional<std::size_t> instruction_override);

std::string format_memory_access_reason(MemorySegmentKind kind, std::size_t addr, std::size_t size,
                                        std::string_view action);

std::string format_bounds_fault_reason(MemorySegmentKind kind, int addr, std::string_view action);

std::string append_segment_reason(std::string_view action, MemorySegmentKind kind, std::size_t addr,
                                  std::string_view base_reason);

}  // namespace t81::vm::internal
