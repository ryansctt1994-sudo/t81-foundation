#include "t81/core/T81Symbol.hpp"
#include <cassert>
#include <iostream>

using namespace t81;

int main() {
    std::cout << "Running T81Symbol tests...\n";

    // Default construction (invalid symbol)
    [[maybe_unused]] T81Symbol invalid;
    assert(!invalid.is_valid());

    // Intern symbols
    [[maybe_unused]] T81Symbol sym1= T81Symbol::intern("test_symbol_1");
    [[maybe_unused]] T81Symbol sym2= T81Symbol::intern("test_symbol_2");
    [[maybe_unused]] T81Symbol sym1_again= T81Symbol::intern("test_symbol_1");

    // Same string should produce same symbol
    assert(sym1 == sym1_again);
    assert(sym1 != sym2);

    // Valid symbols
    assert(sym1.is_valid());
    assert(sym2.is_valid());

    // Hash
    assert(sym1.hash() == sym1_again.hash());
    assert(sym1.hash() != sym2.hash());

    // From raw/id
    [[maybe_unused]] T81Symbol::id_t id= sym1.id();
    [[maybe_unused]] T81Symbol from_id= T81Symbol::from_id(id);
    assert(from_id == sym1);

    // Comparison
    assert(sym1 <= sym1);
    assert(sym1 >= sym1);

    std::cout << "All T81Symbol tests PASSED!\n";
    return 0;
}

