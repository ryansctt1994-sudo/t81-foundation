#include "debugger.hpp"
#include "t81/cli/logging.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace t81::cli {

Debugger::Debugger(std::unique_ptr<t81::vm::IVirtualMachine> vm)
    : vm_(std::move(vm)) {}

void Debugger::run() {
    info("HanoiVM Debugger active. Type 'h' for help.");

    std::string line;
    while (!quit_ && !vm_->state().halted) {
        print_current_instruction();
        std::cout << "dbg> " << std::flush;
        if (!std::getline(std::cin, line)) break;

        if (line.empty()) continue;

        std::istringstream iss(line);
        char cmd;
        iss >> cmd;

        switch (cmd) {
            case 'h':
                print_help();
                break;
            case 's': {
                auto res = vm_->step();
                if (!res) {
                    error("Execution trapped: " + std::string(t81::vm::to_string(res.error())));
                }
                break;
            }
            case 'p': {
                std::string pattern;
                if (iss >> pattern) {
                    policy_breakpoints_.insert(pattern);
                    info("Policy breakpoint set for: " + pattern);
                } else {
                    error("Usage: p <pattern>");
                }
                break;
            }
            case 'c': {
                while (!vm_->state().halted) {
                    auto res = vm_->step();
                    if (!res) {
                        error("Execution trapped: " + std::string(t81::vm::to_string(res.error())));
                        break;
                    }
                    if (breakpoints_.count(vm_->state().pc)) {
                        info("Breakpoint hit at PC=" + std::to_string(vm_->state().pc));
                        break;
                    }
                    bool policy_hit = false;
                    const auto& log = vm_->state().axion_log;
                    if (!log.empty()) {
                        const auto& last_event = log.back();
                        for (const auto& pat : policy_breakpoints_) {
                            if (last_event.verdict.reason.find(pat) != std::string::npos) {
                                info("Policy breakpoint hit: " + pat);
                                info("Reason: " + last_event.verdict.reason);
                                policy_hit = true;
                                break;
                            }
                        }
                    }
                    if (policy_hit) break;
                }
                break;
            }
            case 'b': {
                std::size_t pc;
                if (iss >> pc) {
                    breakpoints_.insert(pc);
                    info("Breakpoint set at PC=" + std::to_string(pc));
                } else {
                    error("Usage: b <pc>");
                }
                break;
            }
            case 'r':
                print_registers();
                break;
            case 'k':
                print_stack();
                break;
            case 'm': {
                std::size_t addr;
                if (iss >> addr) {
                    print_memory(addr);
                } else {
                    error("Usage: m <addr>");
                }
                break;
            }
            case 'q':
                quit_ = true;
                break;
            default:
                error("Unknown command. Type 'h' for help.");
        }
    }

    if (vm_->state().halted) {
        info("Program halted.");
    }
}

void Debugger::print_help() {
    std::cout << "Debugger commands:\n"
              << "  s          Step one instruction\n"
              << "  c          Continue execution\n"
              << "  b <pc>     Set breakpoint at PC\n"
              << "  p <pat>    Set policy breakpoint (on trace string match)\n"
              << "  r          Print registers\n"
              << "  k          Print stack\n"
              << "  m <addr>   Print memory at address\n"
              << "  h          Show this help\n"
              << "  q          Quit debugger\n";
}

void Debugger::print_registers() {
    const auto& state = vm_->state();
    std::cout << "Registers:\n";
    for (int i = 0; i < 9; ++i) { // Show first 9 for brevity
        std::cout << "  R" << i << ": " << state.registers[i] << "\n";
    }
    std::cout << "  PC: " << state.pc << "\n";
    std::cout << "  SP: " << state.sp << "\n";
}

void Debugger::print_stack() {
    const auto& state = vm_->state();
    std::cout << "Stack (top 10):\n";
    std::size_t count = 0;
    for (std::size_t i = state.sp; i > 0 && count < 10; --i, ++count) {
        std::cout << "  [" << i - 1 << "]: " << state.memory[i - 1] << "\n";
    }
}

void Debugger::print_current_instruction() {
    const auto& state = vm_->state();
    if (state.halted) return;
    std::cout << "[" << std::setw(4) << state.pc << "] ";
}

void Debugger::print_memory(std::size_t addr) {
    const auto& state = vm_->state();
    if (addr >= state.memory.size()) {
        error("Address out of bounds");
        return;
    }
    std::cout << "Memory[" << addr << "]: " << state.memory[addr] << "\n";
}

} // namespace t81::cli
