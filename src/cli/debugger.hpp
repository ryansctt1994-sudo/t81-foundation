#pragma once

#include "t81/vm/vm.hpp"
#include "t81/tisc/program.hpp"
#include <set>
#include <string>

namespace t81::cli {

class Debugger {
public:
    Debugger(std::unique_ptr<t81::vm::IVirtualMachine> vm, t81::tisc::Program program);
    void run();

private:
    void print_help();
    void print_registers();
    void print_stack();
    void print_current_instruction();
    void print_list();
    void print_memory(std::size_t addr);

    std::unique_ptr<t81::vm::IVirtualMachine> vm_;
    t81::tisc::Program program_;
    std::set<std::size_t> breakpoints_;
    std::set<std::string> policy_breakpoints_;
    bool quit_{false};
};

} // namespace t81::cli
