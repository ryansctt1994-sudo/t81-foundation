#include <cassert>
#include <iostream>
#include "t81/core/T81Agent.hpp"
#include "t81/core/T81Proof.hpp"

using namespace t81;

void test_proof_basic() {
  auto goal = T81Tree<T81Symbol>::leaf(T81Symbol::intern("Truth"));
  T81Theorem th(T81Symbol::intern("TrueTheorem"), T81String("THE TRUTH"), goal);

  T81Proof proof(th);
  auto agent = T81Agent(T81Symbol::intern("Prover"));
  T81List<T81Entropy> fuel;
  fuel.push_back(acquire_entropy());
  agent.receive_fuel(std::move(fuel));

  T81InferenceRule rule{T81Symbol::intern("Reveal"), goal, nullptr};

  assert(!proof.is_complete());
  bool ok = proof.apply_rule(rule, agent);
  assert(ok);
  assert(proof.is_complete());

  std::cout << "test_proof_basic PASSED\n";
}

int main() {
  test_proof_basic();
  std::cout << "All T81Proof tests PASSED!\n";
  return 0;
}
