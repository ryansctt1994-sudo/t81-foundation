#include <cassert>
#include <iostream>
#include "t81/core/T81Tensor.hpp"

int main() {
    using namespace t81;

    // Rank 2
    T81Tensor<float81, 2, 2, 3> t2;
    // fill t2...
    [[maybe_unused]] auto t2t= transpose(t2);
    static_assert(t2t.shape()[0] == 3);
    static_assert(t2t.shape()[1] == 2);

    // Rank 3
    T81Tensor<float81, 3, 2, 3, 4> t3;
    [[maybe_unused]] auto t3t= transpose(t3);
    static_assert(t3t.shape()[0] == 4);
    static_assert(t3t.shape()[1] == 3);
    static_assert(t3t.shape()[2] == 2);

    // Rank 4
    T81Tensor<float81, 4, 2, 3, 4, 5> t4;
    [[maybe_unused]] auto t4t= transpose(t4);
    static_assert(t4t.shape()[0] == 5);
    static_assert(t4t.shape()[1] == 4);
    static_assert(t4t.shape()[2] == 3);
    static_assert(t4t.shape()[3] == 2);

    std::cout << "T81Tensor transpose static tests passed!\n";
    return 0;
}
