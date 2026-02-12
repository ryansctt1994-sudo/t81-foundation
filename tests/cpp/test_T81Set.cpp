#undef NDEBUG
#include "t81/core/T81Set.hpp"
#include "t81/core/T81Symbol.hpp"
#include <cassert>
#include <iostream>

using namespace t81;

void test_basic() {
    T81Set<T81Symbol> empty;
    assert(empty.empty());
    assert(empty.size() == 0);

    auto sym1 = T81Symbol::intern("sym1");
    auto sym2 = T81Symbol::intern("sym2");
    auto sym3 = T81Symbol::intern("sym3");

    auto set1 = empty.insert(sym1);
    assert(set1.size() == 1);
    assert(set1.contains(sym1));

    auto set2 = set1.insert(sym2);
    assert(set2.size() == 2);
    assert(set2.contains(sym1));
    assert(set2.contains(sym2));

    T81Set<T81Symbol> set3{sym1, sym2, sym3};
    assert(set3.size() == 3);

    auto set4 = set3.erase(sym2);
    assert(set4.size() == 2);
    assert(!set4.contains(sym2));
    assert(set4.contains(sym1));
    assert(set4.contains(sym3));

    std::cout << "test_basic PASSED\n";
}

void test_set_ops() {
    auto a = T81Symbol::intern("a");
    auto b = T81Symbol::intern("b");
    auto c = T81Symbol::intern("c");

    T81Set<T81Symbol> setA{a, b};
    T81Set<T81Symbol> setB{b, c};

    // Union
    auto u = setA | setB;
    assert(u.size() == 3);
    assert(u.contains(a));
    assert(u.contains(b));
    assert(u.contains(c));

    // Intersection
    auto i = setA & setB;
    assert(i.size() == 1);
    assert(i.contains(b));
    assert(!i.contains(a));
    assert(!i.contains(c));

    // Difference
    auto d = setA - setB; // a, b - b, c = a
    assert(d.size() == 1);
    assert(d.contains(a));
    assert(!d.contains(b));

    // Symmetric difference
    // (a, b) sym_diff (b, c) = (a, c)
    auto sd = setA.symmetric_difference(setB);
    assert(sd.size() == 2);
    assert(sd.contains(a));
    assert(sd.contains(c));
    assert(!sd.contains(b));

    std::cout << "test_set_ops PASSED\n";
}

void test_subset_superset() {
    auto a = T81Symbol::intern("a");
    auto b = T81Symbol::intern("b");

    T81Set<T81Symbol> setA{a};
    T81Set<T81Symbol> setAB{a, b};

    assert(setA.subset_of(setAB));
    assert(!setAB.subset_of(setA));

    assert(setAB.superset_of(setA));
    assert(!setA.superset_of(setAB));

    assert(setA.subset_of(setA)); // Reflexive

    std::cout << "test_subset_superset PASSED\n";
}

void test_conversion() {
    auto a = T81Symbol::intern("a");
    auto b = T81Symbol::intern("b");
    T81Set<T81Symbol> set{a, b};

    auto list = set.to_list();
    assert(list.size() == 2);
    // Order is hash-dependent, so check contents
    bool foundA = false, foundB = false;
    for (const auto& s : list) {
        if (s == a) foundA = true;
        if (s == b) foundB = true;
    }
    assert(foundA);
    assert(foundB);

    std::cout << "test_conversion PASSED\n";
}

void test_iterators() {
    auto a = T81Symbol::intern("a");
    T81Set<T81Symbol> set{a};

    int count = 0;
    for (auto it = set.begin(); it != set.end(); ++it) {
        assert(*it == a);
        count++;
    }
    assert(count == 1);

    std::cout << "test_iterators PASSED\n";
}

int main() {
    test_basic();
    test_set_ops();
    test_subset_superset();
    test_conversion();
    test_iterators();
    std::cout << "All T81Set tests PASSED!\n";
    return 0;
}
