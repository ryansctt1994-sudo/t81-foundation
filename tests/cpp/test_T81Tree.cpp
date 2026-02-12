#undef NDEBUG
#include "t81/core/T81Tree.hpp"
#include "t81/core/T81Int.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace t81;

template <typename T>
bool is_leaf(const T81Tree<T>& node) {
    return std::all_of(node.children().begin(), node.children().end(), [](const auto& child) {
        return child == nullptr;
    });
}

void test_basic() {
    auto leaf = T81Tree<T81Int<27>>::leaf(T81Int<27>(42));
    assert(is_leaf(*leaf));
    assert(leaf->value().to_int64() == 42);

    auto node = T81Tree<T81Int<27>>::node(
        T81Int<27>(20),
        T81Tree<T81Int<27>>::leaf(T81Int<27>(10)),
        std::nullopt,
        T81Tree<T81Int<27>>::leaf(T81Int<27>(30))
    );

    assert(!is_leaf(*node));
    assert(node->value().to_int64() == 20);
    assert(node->left() != nullptr);
    assert(node->middle() == nullptr);
    assert(node->right() != nullptr);

    std::cout << "test_basic PASSED\n";
}

void test_equality() {
    auto a = T81Tree<int>::leaf(1);
    auto b = T81Tree<int>::leaf(1);
    auto c = T81Tree<int>::leaf(2);

    assert(*a == *b);
    assert(!(*a == *c));

    auto nodeA = T81Tree<int>::node(10, a, std::nullopt, c);
    auto nodeB = T81Tree<int>::node(10, b, std::nullopt, c);
    auto nodeC = T81Tree<int>::node(10, a, c, std::nullopt);

    assert(*nodeA == *nodeB); // Structure equality
    assert(!(*nodeA == *nodeC)); // Different structure

    // Shared structure equality
    auto nodeShared = nodeA->with_middle(c);
    // nodeShared has same left/right as nodeA, but middle is c.
    // nodeA middle is null.
    assert(!(*nodeShared == *nodeA));
    assert(nodeShared->left() == nodeA->left()); // Pointers should be identical (shared)

    std::cout << "test_equality PASSED\n";
}

void test_deep_tree() {
    // Construct a deep tree (linked list via left child)
    auto root = T81Tree<int>::leaf(0);
    for (int i = 1; i <= 100; ++i) {
        root = T81Tree<int>::node(i, root, std::nullopt, std::nullopt);
    }

    // Check depth
    int depth = 0;
    auto curr = root;
    while (curr) {
        depth++;
        curr = curr->left();
    }
    assert(depth == 101); // 0 to 100

    std::cout << "test_deep_tree PASSED\n";
}

int main() {
    test_basic();
    test_equality();
    test_deep_tree();
    std::cout << "All T81Tree tests PASSED!\n";
    return 0;
}
