#include "t81/tensor.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

#include <cmath>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace {

using t81::T729Tensor;
using t81::tisc::Insn;
using t81::tisc::LiteralKind;
using t81::tisc::Opcode;
using t81::tisc::Program;

Insn make_tensor_handle_imm(int reg, std::int64_t handle) {
  Insn insn{Opcode::LoadImm, reg, handle, 0};
  insn.literal_kind = LiteralKind::TensorHandle;
  return insn;
}

Program make_hot_tensor_program() {
  Program p;
  p.tensor_pool.push_back(T729Tensor({2, 2}, {1.0f, 2.0f, 3.0f, 4.0f}));       // handle 1
  p.tensor_pool.push_back(T729Tensor({2, 2}, {0.5f, 0.0f, 1.0f, -0.5f}));      // handle 2
  p.tensor_pool.push_back(T729Tensor({2}, {1.0f, 1.0f}));                       // handle 3

  p.insns.push_back(make_tensor_handle_imm(1, 1));
  p.insns.push_back(make_tensor_handle_imm(2, 2));
  p.insns.push_back(make_tensor_handle_imm(3, 3));
  p.insns.push_back({Opcode::LoadImm, 4, 64, 0}); // loop counter

  // loop pc = 4
  p.insns.push_back({Opcode::TMatMul, 5, 1, 2});
  p.insns.push_back({Opcode::TRMSNorm, 6, 5, 3});
  p.insns.push_back({Opcode::Mov, 1, 6, 0});      // feed result into next step
  p.insns.push_back({Opcode::Dec, 4, 0, 0});
  p.insns.push_back({Opcode::JumpIfNotZero, 4, 4, 0});
  p.insns.push_back({Opcode::Halt, 0, 0, 0});
  return p;
}

struct Snapshot {
  std::vector<std::int64_t> regs;
  std::vector<t81::vm::ValueTag> tags;
  T729Tensor final_tensor;
  std::vector<std::string> reasons;
};

Snapshot run_once(const Program& p) {
  auto vm = t81::vm::make_interpreter_vm();
  vm->load_program(p);
  [[maybe_unused]] auto run = vm->run_to_halt();
  assert(run.has_value());

  Snapshot out;
  const auto& st = vm->state();
  out.regs.assign(st.registers.begin(), st.registers.end());
  out.tags.assign(st.register_tags.begin(), st.register_tags.end());
  const auto final_handle = st.registers[6];
  assert(final_handle > 0);
  const auto idx = static_cast<size_t>(final_handle - 1);
  assert(idx < st.tensors.size());
  out.final_tensor = st.tensors[idx];
  for (const auto& entry : st.axion_log) {
    out.reasons.push_back(entry.verdict.reason);
  }
  return out;
}

void assert_tensor_close(const T729Tensor& a, const T729Tensor& b) {
  assert(a.shape() == b.shape());
  assert(a.data().size() == b.data().size());
  for (size_t i = 0; i < a.data().size(); ++i) {
    const float diff = std::abs(a.data()[i] - b.data()[i]);
    if (diff > 1e-5f) {
      std::cerr << "tensor mismatch at " << i << ": " << a.data()[i] << " vs " << b.data()[i]
                << " diff=" << diff << "\n";
      assert(false);
    }
  }
}

void test_jit_tensor_trace_equivalence_and_boundary_logs() {
  const Program p = make_hot_tensor_program();
  const auto a = run_once(p);
  const auto b = run_once(p);

  assert(a.regs == b.regs);
  assert(a.tags == b.tags);
  assert_tensor_close(a.final_tensor, b.final_tensor);
  assert(a.reasons == b.reasons);

  bool saw_enter = false;
  bool saw_exit = false;
  for (const auto& reason : a.reasons) {
    if (reason.find("jit trace enter") != std::string::npos) {
      saw_enter = true;
    }
    if (reason.find("jit trace exit") != std::string::npos) {
      saw_exit = true;
    }
  }
  if (!saw_enter || !saw_exit) {
    std::cerr << "missing jit tensor boundary reasons: enter=" << saw_enter
              << " exit=" << saw_exit << "\n";
    assert(false);
  }
}

}  // namespace

int main() {
  test_jit_tensor_trace_equivalence_and_boundary_logs();
  std::cout << "jit tensor trace equivalence test passed\n";
  return 0;
}
