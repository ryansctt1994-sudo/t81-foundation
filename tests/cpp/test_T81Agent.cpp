#undef NDEBUG
#include <cassert>
#include <iostream>
#include "t81/core/T81Agent.hpp"
#include "t81/core/T81Entropy.hpp"

using namespace t81;

void test_identity() {
  auto id = T81Symbol::intern("Socrates");
  T81Agent agent(id);
  assert(agent.identity() == id);
  std::cout << "test_identity PASSED\n";
}

void test_fuel() {
  auto id = T81Symbol::intern("Prometheus");
  // Create initial fuel
  T81List<T81Entropy> fuel;
  fuel.push_back(acquire_entropy());
  fuel.push_back(acquire_entropy());

  T81Agent agent(id, std::move(fuel));
  assert(agent.fuel_remaining() == 2);

  auto token = agent.consume_entropy();
  assert(token.has_value());
  assert(agent.fuel_remaining() == 1);

  // Add more fuel
  T81List<T81Entropy> more_fuel;
  more_fuel.push_back(acquire_entropy());
  agent.receive_fuel(std::move(more_fuel));
  assert(agent.fuel_remaining() == 2);

  std::cout << "test_fuel PASSED\n";
}

void test_belief() {
  auto id = T81Symbol::intern("Plato");
  T81List<T81Entropy> fuel;
  fuel.push_back(acquire_entropy());
  fuel.push_back(acquire_entropy());
  fuel.push_back(acquire_entropy());
  fuel.push_back(acquire_entropy());
  fuel.push_back(acquire_entropy());  // Plenty of fuel
  T81Agent agent(id, std::move(fuel));

  auto idea = T81Symbol::intern("Forms");
  auto conf = T81Prob27::from_prob(0.8);

  // Initial belief is 0
  auto initial = agent.belief(idea);
  assert(initial.to_prob() < 0.001);  // 0.0

  // Set a prior of 0.5 (uncertainty) to avoid -inf issues and verify update logic
  agent.believe(idea, T81Prob27::from_prob(0.5));

  // Observe 0.8
  agent.observe(idea, conf);
  auto updated = agent.belief(idea);

  // Original was 0.5 (log-odds 0).
  // Target was 0.8 (log-odds > 0).
  // Step size is from_prob(0.55) -> log-odds ~0.2.
  // Updated log-odds = 0 + 0.2 = 0.2.
  // Prob(0.2) ≈ 0.55.
  // So updated probability should be around 0.55.
  double p = updated.to_prob();
  std::cout << "Observed belief: " << p << std::endl;
  assert(p > 0.54 && p < 0.56);

  // Believe directly sets belief
  agent.believe(idea, conf);
  auto direct = agent.belief(idea);
  assert(direct.to_prob() > 0.79 && direct.to_prob() < 0.81);

  std::cout << "test_belief PASSED\n";
}

void test_act() {
  auto id = T81Symbol::intern("Aristotle");
  T81List<T81Entropy> fuel;
  fuel.push_back(acquire_entropy());
  T81Agent agent(id, std::move(fuel));

  auto initial_intent = agent.intent();
  // Identity quaternion w=1, x=0, y=0, z=0
  assert(initial_intent.w().to_double() > 0.9);

  agent.act();  // Consumes fuel, rotates intent

  auto new_intent = agent.intent();
  // Should have rotated slightly
  assert(new_intent.w().to_double() < 0.999);
  assert(new_intent != initial_intent);

  std::cout << "test_act PASSED\n";
}

void test_remember() {
  auto id = T81Symbol::intern("MemoryMan");
  T81List<T81Entropy> fuel;
  fuel.push_back(acquire_entropy());
  T81Agent agent(id, std::move(fuel));

  auto parent = T81Symbol::intern("Root");
  auto child = T81Symbol::intern("Leaf");

  agent.remember(parent, child);

  const auto& mem = agent.memory();
  assert(mem.value() == parent);
  // memory_root_ structure from remember:
  // node(parent, nullopt, child_node, nullopt) (middle child)
  // Wait, implementation: memory_root_ = SymbolTree::node(parent, nullopt, child_node, nullopt)
  // child_node is leaf(child).
  // So mem.middle() should be valid.
  assert(mem.middle() != nullptr);
  assert(mem.middle()->value() == child);

  std::cout << "test_remember PASSED\n";
}

void test_thought_stream() {
  auto id = T81Symbol::intern("Thinker");
  T81List<T81Entropy> fuel;
  fuel.push_back(acquire_entropy());
  T81Agent agent(id, std::move(fuel));

  auto stream = agent.thought_stream();
  auto it = stream.begin();

  // First thought
  // "I AM Thinker | FUEL:1 | BELIEF IN SELF:1.000000"
  std::string s = *it;
  std::cout << "Thought: " << s << std::endl;
  assert(s.find("I AM Thinker") != std::string::npos);
  assert(s.find("FUEL:1") != std::string::npos);

  std::cout << "test_thought_stream PASSED\n";
}

int main() {
  test_identity();
  test_fuel();
  test_belief();
  test_act();
  test_remember();
  test_thought_stream();
  std::cout << "All T81Agent tests PASSED!\n";
  return 0;
}
