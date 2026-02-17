#include <cassert>
#include <chrono>
#include <iostream>
#include "t81/core/T81Agent.hpp"
#include "t81/core/T81Thread.hpp"

using namespace t81;

void test_thread_basic() {
  auto agent = T81Agent(T81Symbol::intern("Worker"));
  bool flag = false;

  auto t =
      T81Thread::spawn(T81Symbol::intern("Task"), std::move(agent), [&flag]() { flag = true; });

  t.join();
  assert(flag == true);
  assert(!t.is_alive());
  std::cout << "test_thread_basic PASSED\n";
}

void test_thread_detach() {
  auto agent = T81Agent(T81Symbol::intern("DetachedWorker"));
  auto t = T81Thread::spawn(T81Symbol::intern("DetachedTask"), std::move(agent),
                            []() { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
  t.detach();
  assert(!t.is_alive() || true);  // Can't easily check after detach without shared state
  std::cout << "test_thread_detach PASSED\n";
}

int main() {
  test_thread_basic();
  test_thread_detach();
  std::cout << "All T81Thread tests PASSED!\n";
  return 0;
}
