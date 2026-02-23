#undef NDEBUG
#include <cassert>
#include <iostream>
#include <vector>
#include "t81/core/T81Agent.hpp"
#include "t81/core/T81Entropy.hpp"
#include "t81/tensor.hpp"

using namespace t81;

void test_neural_identity() {
  auto id = T81Symbol::intern("NeuralAgent");
  T81List<T81Entropy> fuel;
  fuel.push_back(acquire_entropy());
  fuel.push_back(acquire_entropy());
  T81Agent agent(id, std::move(fuel));

  // Create input tensor [1, 2, 3]
  std::vector<float> data = {1.0f, 2.0f, 3.0f};
  T729DynamicTensor input({3}, data);

  // Infer
  T729DynamicTensor output = agent.infer(input);

  // Verify identity
  assert(output.shape() == input.shape());
  assert(output.data() == input.data());

  std::cout << "test_neural_identity PASSED\n";
}

void test_train_stub() {
  auto id = T81Symbol::intern("Learner");
  T81List<T81Entropy> fuel;
  fuel.push_back(acquire_entropy());
  T81Agent agent(id, std::move(fuel));

  T729DynamicTensor input({1}, {1.0f});
  T729DynamicTensor target({1}, {1.0f});

  // Should not crash
  agent.train(input, target);

  std::cout << "test_train_stub PASSED\n";
}

int main() {
  test_neural_identity();
  test_train_stub();
  std::cout << "All T81Agent Neural tests PASSED!\n";
  return 0;
}
