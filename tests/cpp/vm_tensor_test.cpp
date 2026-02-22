#include <vector>
#include "test_runtime_check.hpp"

#include "t81/tensor.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

using namespace t81;

int main() {
  [[maybe_unused]] tisc::Program program;
  program.insns.push_back({tisc::Opcode::TVecAdd, 3, 1, 2});
  program.insns.push_back({tisc::Opcode::TMatMul, 4, 5, 6});
  program.insns.push_back({tisc::Opcode::LoadImm, 9, 3, 0});
  program.insns.push_back({tisc::Opcode::I2F, 8, 9, 0});
  program.insns.push_back({tisc::Opcode::F2I, 10, 8, 0});
  program.insns.push_back({tisc::Opcode::I2Frac, 11, 9, 0});
  program.insns.push_back({tisc::Opcode::Frac2I, 12, 11, 0});
  program.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

  [[maybe_unused]] auto vm = vm::make_interpreter_vm();
  vm->load_program(program);

  auto& mutable_state = const_cast<vm::State&>(vm->state());
  // Seed tensor pool with two vectors and two matrices.
  t81::T729Tensor vecA({3}, {1.0f, 2.0f, 3.0f});
  t81::T729Tensor vecB({3}, {4.0f, 5.0f, 6.0f});
  t81::T729Tensor matA({2, 2}, {1.0f, 2.0f, 3.0f, 4.0f});
  t81::T729Tensor matB({2, 2}, {5.0f, 6.0f, 7.0f, 8.0f});
  mutable_state.tensors.push_back(vecA);  // handle 1
  mutable_state.tensors.push_back(vecB);  // handle 2
  mutable_state.tensors.push_back(matA);  // handle 3
  mutable_state.tensors.push_back(matB);  // handle 4
  mutable_state.contexts[0].registers[1] = 1;
  mutable_state.contexts[0].registers[2] = 2;
  mutable_state.contexts[0].registers[5] = 3;
  mutable_state.contexts[0].registers[6] = 4;

  [[maybe_unused]] auto result = vm->run_to_halt();
  T81_TEST_CHECK(result.has_value());

  // Vector addition
  [[maybe_unused]] auto vecHandle = vm->state().contexts[0].registers[3];
  T81_TEST_CHECK(vecHandle == 5);  // 4th tensor inserted next index
  const auto& vecRes = mutable_state.tensors[static_cast<std::size_t>(vecHandle - 1)];
  T81_TEST_CHECK(vecRes.has_value());
  T81_TEST_CHECK(vecRes.value().shape()[0] == 3);
  T81_TEST_CHECK(vecRes.value().data()[0] == 5.0f && vecRes.value().data()[2] == 9.0f);

  // Matrix multiplication
  [[maybe_unused]] auto matHandle = vm->state().contexts[0].registers[4];
  const auto& matRes = mutable_state.tensors[static_cast<std::size_t>(matHandle - 1)];
  T81_TEST_CHECK(matRes.has_value());
  T81_TEST_CHECK(matRes.value().shape()[0] == 2 && matRes.value().shape()[1] == 2);
  T81_TEST_CHECK(static_cast<int>(matRes.value().data()[0]) == 19);  // 1*5 + 2*7

  // Conversion ops
  T81_TEST_CHECK(vm->state().contexts[0].registers[10] == 3);
  T81_TEST_CHECK(vm->state().contexts[0].registers[12] == 3);

  // Shape checks via literal handles.
  [[maybe_unused]] tisc::Program chk;
  chk.tensor_pool.push_back(t81::T729Tensor({2, 2}, {1.0f, 0.0f, 0.0f, 1.0f}));
  chk.shape_pool.push_back({2, 2});
  chk.shape_pool.push_back({2, 3});
  tisc::Insn lt{tisc::Opcode::LoadImm, 1, 1, 0};
  lt.literal_kind = t81::tisc::LiteralKind::TensorHandle;
  chk.insns.push_back(lt);
  tisc::Insn ls{tisc::Opcode::LoadImm, 2, 1, 0};
  ls.literal_kind = t81::tisc::LiteralKind::ShapeHandle;
  chk.insns.push_back(ls);
  chk.insns.push_back({tisc::Opcode::ChkShape, 3, 1, 2});
  tisc::Insn ls_bad{tisc::Opcode::LoadImm, 4, 2, 0};
  ls_bad.literal_kind = t81::tisc::LiteralKind::ShapeHandle;
  chk.insns.push_back(ls_bad);
  chk.insns.push_back({tisc::Opcode::ChkShape, 5, 1, 4});
  chk.insns.push_back({tisc::Opcode::Halt, 0, 0, 0});

  [[maybe_unused]] auto vm_chk = vm::make_interpreter_vm();
  vm_chk->load_program(chk);
  [[maybe_unused]] auto res_chk = vm_chk->run_to_halt();
  T81_TEST_CHECK(res_chk.has_value());
  T81_TEST_CHECK(vm_chk->state().contexts[0].registers[3] == 1);
  T81_TEST_CHECK(vm_chk->state().contexts[0].registers[5] == 0);

  return 0;
}
