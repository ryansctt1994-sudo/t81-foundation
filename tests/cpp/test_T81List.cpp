#undef NDEBUG
#include "t81/core/T81List.hpp"
#include "t81/core/T81Int.hpp"
#include "t81/core/T81Entropy.hpp"
#include <cassert>
#include <iostream>
#include <vector>

using namespace t81;

void test_basic() {
    T81List<T81Int<27>> list{T81Int<27>(1), T81Int<27>(2), T81Int<27>(3)};
    assert(list.size() == 3);
    assert(list[0].to_int64() == 1);
    list.push_back(T81Int<27>(4));
    assert(list.size() == 4);
    assert(list.back().to_int64() == 4);
    std::cout << "test_basic PASSED\n";
}

void test_modifiers() {
    T81List<int> list;
    list.reserve(10);
    assert(list.capacity() >= 10);

    list.emplace_back(10);
    list.emplace_back(20);
    assert(list.size() == 2);
    assert(list[0] == 10);
    assert(list[1] == 20);

    list.pop_back();
    assert(list.size() == 1);
    assert(list.back() == 10);

    list.clear();
    assert(list.empty());

    list.shrink_to_fit();
    // Capacity might or might not be 0, but it should work
    std::cout << "test_modifiers PASSED\n";
}

void test_move_only() {
    // T81Entropy is move-only
    T81List<T81Entropy> list;
    list.push_back(acquire_entropy());
    list.push_back(acquire_entropy());
    assert(list.size() == 2);

    T81List<T81Entropy> other;
    other.push_back(acquire_entropy());

    // Concatenation with move
    // list += std::move(other);
    // T81List::operator+= handles rvalue ref
    list += std::move(other);
    assert(list.size() == 3);
    assert(other.empty()); // moved from

    // Move construction
    T81List<T81Entropy> moved(std::move(list));
    assert(moved.size() == 3);
    assert(list.empty()); // vector moved

    std::cout << "test_move_only PASSED\n";
}

void test_iterators() {
    T81List<int> list{10, 20, 30};
    int sum = 0;
    for (auto x : list) {
        sum += x;
    }
    assert(sum == 60);

    // Const iterator
    const auto& clist = list;
    sum = 0;
    for (auto x : clist) {
        sum += x;
    }
    assert(sum == 60);

    std::cout << "test_iterators PASSED\n";
}

void test_span() {
    T81List<int> list{1, 2, 3};
    auto sp = list.span();
    assert(sp.size() == 3);
    assert(sp[1] == 2);

    // Modification via span
    sp[1] = 99;
    assert(list[1] == 99);

    std::cout << "test_span PASSED\n";
}

int main() {
    test_basic();
    test_modifiers();
    test_move_only();
    test_iterators();
    test_span();
    std::cout << "All T81List tests PASSED!\n";
    return 0;
}
