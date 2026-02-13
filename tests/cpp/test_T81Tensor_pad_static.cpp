#include "t81/core/T81Tensor.hpp"
#include <cassert>
#include <iostream>

using namespace t81;

int main() {
    // 2x3 Tensor
    // [1 1 1]
    // [1 1 1]
    using Tensor2x3 = T81Tensor<T81Float<72,9>, 2, 2, 3>;
    Tensor2x3 t(T81Float<72,9>(1.0));

    // Pad dim0: 1 before, 0 after
    // Pad dim1: 1 before, 1 after
    // Output: (2+1+0) x (3+1+1) = 3x5

    // [0 0 0 0 0]
    // [0 1 1 1 0]
    // [0 1 1 1 0]

    auto p = pad<1, 0, 1, 1>(t, T81Float<72,9>(0.0));

    static_assert(decltype(p)::rank() == 2);
    static_assert(decltype(p)::shape()[0] == 3);
    static_assert(decltype(p)::shape()[1] == 5);

    assert(p(0, 0).to_double() == 0.0);
    assert(p(1, 1).to_double() == 1.0);
    assert(p(2, 3).to_double() == 1.0);
    assert(p(2, 4).to_double() == 0.0);

    std::cout << "pad test passed" << std::endl;
    return 0;
}
