#undef NDEBUG
#include <cassert>
#include <iostream>
#include <vector>
#include "t81/types/T81Stream.hpp"

using namespace t81;

void test_naturals() {
  auto n = naturals();
  auto it = n.begin();
  assert((*it).to_int64() == 0);
  ++it;
  assert((*it).to_int64() == 1);
  ++it;
  assert((*it).to_int64() == 2);
  std::cout << "test_naturals PASSED\n";
}

void test_fibonacci() {
  auto fib = fibonacci();
  auto it = fib.begin();

  // 0, 1, 1, 2, 3, 5, 8
  assert((*it).to_int64() == 0);
  ++it;
  assert((*it).to_int64() == 1);
  ++it;
  assert((*it).to_int64() == 1);
  ++it;
  assert((*it).to_int64() == 2);
  ++it;
  assert((*it).to_int64() == 3);
  ++it;
  assert((*it).to_int64() == 5);
  ++it;
  assert((*it).to_int64() == 8);

  std::cout << "test_fibonacci PASSED\n";
}

void test_constant() {
  auto cons = constant(42);
  auto it = cons.begin();
  assert(*it == 42);
  ++it;
  assert(*it == 42);
  ++it;
  assert(*it == 42);

  std::cout << "test_constant PASSED\n";
}

void test_stream_from() {
  int counter = 0;
  auto s = stream_from([&counter]() mutable { return ++counter; });
  auto it = s.begin();

  assert(*it == 1);
  ++it;
  assert(*it == 2);
  ++it;
  assert(*it == 3);

  std::cout << "test_stream_from PASSED\n";
}

void test_finite_stream() {
  // T81Stream supports infinite streams.
  // Can we make a finite stream?
  // If coroutine returns (co_return), promise.return_void() is called.
  // T81Stream::iterator handles done() check.

  auto finite = []() -> T81Stream<int> {
    co_yield 1;
    co_yield 2;
    co_yield 3;
  };

  auto s = finite();
  std::vector<int> values;
  for (auto val : s) {
    values.push_back(val);
  }

  std::cout << "Values size: " << values.size() << std::endl;
  assert(values.size() == 3);
  assert(values[0] == 1);
  assert(values[1] == 2);
  assert(values[2] == 3);

  std::cout << "test_finite_stream PASSED\n";
}

int main() {
  test_naturals();
  test_fibonacci();
  test_constant();
  test_stream_from();
  test_finite_stream();
  std::cout << "All T81Stream tests PASSED!\n";
  return 0;
}
