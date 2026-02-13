#include "t81/core/T81Tensor.hpp"
#include <cassert>
#include <iostream>

using namespace t81;

int main() {
    using Tensor2x3 = T81Tensor<T81Float<72,9>, 2, 2, 3>;
    using Tensor1x3 = T81Tensor<T81Float<72,9>, 2, 1, 3>;

    Tensor2x3 t1;
    for(size_t i=0; i<6; ++i) t1.data[i] = T81Float<72,9>(static_cast<double>(i)); // 0..5

    Tensor1x3 t2;
    for(size_t i=0; i<3; ++i) t2.data[i] = T81Float<72,9>(static_cast<double>(6 + i)); // 6..8

    // Concat t1 and t2 along axis 0 -> 3x3
    auto c = concat(t1, t2);

    static_assert(decltype(c)::rank() == 2);
    static_assert(decltype(c)::shape()[0] == 3);
    static_assert(decltype(c)::shape()[1] == 3);

    for(size_t i=0; i<9; ++i) {
        assert(c.data[i].to_double() == static_cast<double>(i));
    }

    std::cout << "concat test passed" << std::endl;
    return 0;
}
