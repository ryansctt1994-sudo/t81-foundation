import os

# Fix include/t81/vm/vm.hpp
vm_hpp_path = 'include/t81/vm/vm.hpp'
vm_hpp_content = """#pragma once

#include <memory>
#include <string_view>
#include <t81/support/expected.hpp>
#include "t81/axion/engine.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/state.hpp"
#include "t81/vm/traps.hpp"

#include <vector>

namespace t81::vm {

class IVirtualMachine {
public:
  virtual ~IVirtualMachine() = default;
  virtual void load_program(const t81::tisc::Program& program) = 0;
  virtual void set_fault_injections(std::vector<FaultInjection> faults) = 0;
  virtual std::expected<void, Trap> step() = 0;
  virtual std::expected<void, Trap> run_to_halt(std::size_t max_steps = 100000) = 0;
  virtual const State& state() const = 0;
  virtual void set_register(int idx, std::int64_t value, ValueTag tag = ValueTag::Int) = 0;
  virtual std::int64_t load_weights_tensor(std::string_view name) = 0;
  virtual const t81::weights::NativeTensor* weights_tensor(std::int64_t handle) const = 0;
};

// Factory for the in-tree interpreter implementation.
std::unique_ptr<IVirtualMachine> make_interpreter_vm(
    std::unique_ptr<t81::axion::Engine> engine = nullptr);
}  // namespace t81::vm
"""
with open(vm_hpp_path, 'w') as f:
    f.write(vm_hpp_content)
print(f"Fixed {vm_hpp_path}")

# Fix core/vm/vm.cpp
vm_cpp_path = 'core/vm/vm.cpp'
with open(vm_cpp_path, 'r') as f:
    lines = f.readlines()

new_lines = []
i = 0
while i < len(lines):
    line = lines[i]
    if 'void set_fault_injections(std::vector<FaultInjection> faults) override {' in line:
        # Check if next line has fault_injections_ assignment (the bad one)
        if i+1 < len(lines) and 'fault_injections_ = std::move(faults);' in lines[i+1]:
            print(f"Removing duplicate method at line {i+1}")
            i += 3 # Skip 3 lines
            continue

    # Check for the member variable at the end
    if 'std::vector<FaultInjection> fault_injections_;' in line:
        print(f"Removing member variable at line {i+1}")
        i += 1
        continue

    new_lines.append(line)
    i += 1

with open(vm_cpp_path, 'w') as f:
    f.writelines(new_lines)
print(f"Fixed {vm_cpp_path}")
