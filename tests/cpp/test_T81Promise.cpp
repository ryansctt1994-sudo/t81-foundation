#include "t81/core/T81Promise.hpp"
#include "t81/core/T81Agent.hpp"
#include <cassert>
#include <iostream>

using namespace t81;

T81Promise<int> async_value(int x) {
    co_return x;
}

void test_promise_basic() {
    auto p = async_value(42);
    auto agent = T81Agent(T81Symbol::intern("TestAgent"));

    // It should be fulfilled immediately because async_value doesn't await anything
    assert(p.state() == T81Promise<int>::State::FULFILLED);
    auto res = p.try_get();
    assert(res.has_value());
    assert(res.value() == 42);
    std::cout << "test_promise_basic PASSED\n";
}

void test_promise_cancel() {
    // Can't easily test cancellation of something that completes immediately
    // unless we had a promise that suspends.
    std::cout << "test_promise_cancel SKIPPED (TODO: add suspending promise)\n";
}

int main() {
    test_promise_basic();
    test_promise_cancel();
    std::cout << "All T81Promise tests PASSED!\n";
    return 0;
}
