#include "t81/tensor.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using t81::T729Tensor;
using t81::tisc::Insn;
using t81::tisc::LiteralKind;
using t81::tisc::Opcode;
using t81::tisc::Program;

bool expect(bool cond, const std::string& message) {
  if (!cond) {
    std::cerr << "jit_tensor_trace_equivalence_test failure: " << message << "\n";
    return false;
  }
  return true;
}

Insn make_tensor_handle_imm(int reg, std::int64_t handle) {
  Insn insn{Opcode::LoadImm, reg, handle, 0};
  insn.literal_kind = LiteralKind::TensorHandle;
  return insn;
}

Program make_hot_tensor_program() {
  Program p;
  p.tensor_pool.push_back(T729Tensor({2, 2}, {1.0f, 2.0f, 3.0f, 4.0f}));   // handle 1
  p.tensor_pool.push_back(T729Tensor({2, 2}, {0.5f, 0.0f, 1.0f, -0.5f}));  // handle 2
  p.tensor_pool.push_back(T729Tensor({2}, {1.0f, 1.0f}));                  // handle 3

  p.insns.push_back(make_tensor_handle_imm(1, 1));
  p.insns.push_back(make_tensor_handle_imm(2, 2));
  p.insns.push_back(make_tensor_handle_imm(3, 3));
  p.insns.push_back({Opcode::LoadImm, 4, 64, 0});  // loop counter

  // loop pc = 4
  p.insns.push_back({Opcode::TMatMul, 5, 1, 2});
  p.insns.push_back({Opcode::TRMSNorm, 6, 5, 3});
  p.insns.push_back({Opcode::Mov, 1, 6, 0});  // feed result into next step
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
  auto run = vm->run_to_halt();
  if (!run.has_value()) {
    std::cerr << "run_to_halt trap=" << static_cast<int>(run.error()) << "\n";
    return {};
  }

  Snapshot out;
  const auto& st = vm->state();
  out.regs.assign(st.registers.begin(), st.registers.end());
  out.tags.assign(st.register_tags.begin(), st.register_tags.end());
  const auto final_handle = st.registers[6];
  if (final_handle <= 0) {
    std::cerr << "final tensor handle invalid: " << final_handle << "\n";
    return {};
  }
  const auto idx = static_cast<size_t>(final_handle - 1);
  if (idx >= st.tensors.size()) {
    std::cerr << "final tensor handle out of range: " << final_handle << "\n";
    return {};
  }
  out.final_tensor = st.tensors[idx];
  for (const auto& entry : st.axion_log) {
    out.reasons.push_back(entry.verdict.reason);
  }
  return out;
}

bool tensor_close(const T729Tensor& a, const T729Tensor& b) {
  if (!expect(a.shape() == b.shape(), "tensor shapes diverged")) return false;
  if (!expect(a.data().size() == b.data().size(), "tensor data sizes diverged")) return false;
  for (size_t i = 0; i < a.data().size(); ++i) {
    const float diff = std::abs(a.data()[i] - b.data()[i]);
    if (diff > 1e-5f) {
      std::cerr << "tensor mismatch at " << i << ": " << a.data()[i] << " vs " << b.data()[i]
                << " diff=" << diff << "\n";
      return false;
    }
  }
  return true;
}

bool test_jit_tensor_trace_equivalence_and_boundary_logs() {
  const Program p = make_hot_tensor_program();
  const auto a = run_once(p);
  const auto b = run_once(p);

  if (!expect(!a.regs.empty(), "tensor snapshot A is empty")) return false;
  if (!expect(!b.regs.empty(), "tensor snapshot B is empty")) return false;
  if (!expect(a.regs == b.regs, "tensor register snapshots diverged")) return false;
  if (!expect(a.tags == b.tags, "tensor register-tag snapshots diverged")) return false;
  if (!tensor_close(a.final_tensor, b.final_tensor)) return false;
  if (!expect(a.reasons == b.reasons, "tensor trace reasons diverged")) return false;

  bool saw_enter = false;
  bool saw_exit = false;
  bool saw_enter_at_loop_pc = false;
  bool saw_exit_kind = false;
  for (const auto& reason : a.reasons) {
    if (reason.find("jit trace enter") != std::string::npos) {
      saw_enter = true;
      if (reason.find("pc=4") != std::string::npos) {
        saw_enter_at_loop_pc = true;
      }
    }
    if (reason.find("jit trace exit") != std::string::npos) {
      saw_exit = true;
      if (reason.find("exit-kind=") != std::string::npos) {
        saw_exit_kind = true;
      }
    }
  }
  if (!saw_enter || !saw_exit || !saw_enter_at_loop_pc || !saw_exit_kind) {
    std::cerr << "missing jit tensor boundary reasons: enter=" << saw_enter << " exit=" << saw_exit
              << " enter_pc4=" << saw_enter_at_loop_pc << " exit_kind=" << saw_exit_kind << "\n";
    return expect(false, "missing JIT tensor boundary logs");
  }
  return true;
}

}  // namespace

int main() {
  if (!test_jit_tensor_trace_equivalence_and_boundary_logs()) return 1;
  std::cout << "jit tensor trace equivalence test passed\n";
  return 0;
}
