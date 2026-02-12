#undef NDEBUG
#include "t81/core/T81Promise.hpp"
#include "t81/core/T81Agent.hpp"
#include <cassert>
#include <iostream>

using namespace t81;

T81Promise<int> async_value(int x) {
    co_return x;
}

T81Promise<int> suspending_value(int x) {
    co_await acquire_entropy();
    co_return x;
}

void test_promise_basic() {
    auto p = async_value(42);
    // It should be fulfilled immediately
    assert(p.state() == T81Promise<int>::State::FULFILLED);
    auto res = p.try_get();
    assert(res.has_value());
    assert(res.value() == 42);
    std::cout << "test_promise_basic PASSED\n";
}

void test_promise_cancel() {
    auto p = suspending_value(99);

    // Should be pending because it co_awaited entropy and suspended
    assert(p.state() == T81Promise<int>::State::PENDING);

    p.cancel();

    // State should now be CANCELLED
    assert(p.state() == T81Promise<int>::State::CANCELLED);

    // Trying to get value should fail
    auto res = p.try_get();
    assert(!res.has_value());

    // Awaiting should return failure
    T81Agent agent(T81Symbol::intern("TestAgent"));
    auto result = p.await(acquire_entropy(), agent);
    assert(result.is_err());
    // Use raw comparison or symbol interning
    assert(result.error().code == T81Symbol::intern("CANCELLED"));

    std::cout << "test_promise_cancel PASSED\n";
}

int main() {
    test_promise_basic();
    test_promise_cancel();
    std::cout << "All T81Promise tests PASSED!\n";
    return 0;
}
