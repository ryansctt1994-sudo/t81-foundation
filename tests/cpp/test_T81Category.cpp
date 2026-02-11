#include "t81/core/T81Category.hpp"
#include <cassert>
#include <iostream>

using namespace t81;

void test_category_basic() {
    T81Category<int, int> cat;
    cat.add_object(1, T81Symbol::intern("Obj1"));
    cat.add_object(2, T81Symbol::intern("Obj2"));

    // Add a morphism 1 -> 2 (represented by 10)
    cat.add_morphism(T81Symbol::intern("f"), 1, 2, 10);

    // Identity check
    auto id1 = cat.identity_of(1);
    assert(id1 == T81Symbol::intern("Obj1"));

    std::cout << "test_category_basic PASSED\n";
}

int main() {
    test_category_basic();
    std::cout << "All T81Category tests PASSED!\n";
    return 0;
}
