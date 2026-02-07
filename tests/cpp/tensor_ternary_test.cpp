#include "t81/tensor.hpp"
#include "t81/tensor/ops.hpp"
#include <cassert>
#include <iostream>
#include <vector>

using namespace t81;
using namespace t81::ops;

void test_ternary_tensor_basic() {
    T729IntTensor a({2, 3});
    for (int i = 0; i < 6; ++i) a.data()[i] = T81Int<81>(i);

    assert(a.rank() == 2);
    assert(a.shape()[0] == 2);
    assert(a.shape()[1] == 3);
    assert(a.size() == 6);
    assert(a.data()[5].to_int64() == 5);

    std::cout << "test_ternary_tensor_basic passed!" << std::endl;
}

void test_ternary_tensor_ops() {
    T729IntTensor a({2, 2}, {T81Int<81>(1), T81Int<81>(2), T81Int<81>(3), T81Int<81>(4)});
    T729IntTensor b({2, 2}, {T81Int<81>(10), T81Int<81>(20), T81Int<81>(30), T81Int<81>(40)});

    [[maybe_unused]] auto c= add(a, b);
    assert(c.data()[0].to_int64() == 11);
    assert(c.data()[3].to_int64() == 44);

    [[maybe_unused]] auto d= sub(b, a);
    assert(d.data()[0].to_int64() == 9);
    assert(d.data()[3].to_int64() == 36);

    [[maybe_unused]] auto e= mul(a, a);
    assert(e.data()[0].to_int64() == 1);
    assert(e.data()[3].to_int64() == 16);

    std::cout << "test_ternary_tensor_ops passed!" << std::endl;
}

void test_reductions() {
    T729Tensor a({2, 3}, {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f});

    auto min_col = reduce_min_2d(a, 0); // min of [1,4], [2,5], [3,6] -> [1,2,3]
    assert(min_col.data()[0] == 1.0f);
    assert(min_col.data()[2] == 3.0f);

    auto min_row = reduce_min_2d(a, 1); // min of [1,2,3], [4,5,6] -> [1,4]
    assert(min_row.data()[0] == 1.0f);
    assert(min_row.data()[1] == 4.0f);

    auto mean_col = reduce_mean_2d(a, 0); // mean of [1,4], [2,5], [3,6] -> [2.5, 3.5, 4.5]
    assert(mean_col.data()[0] == 2.5f);
    assert(mean_col.data()[2] == 4.5f);

    std::cout << "test_reductions passed!" << std::endl;
}

void test_broadcast_ternary() {
    T729IntTensor a({3}, {T81Int<81>(1), T81Int<81>(2), T81Int<81>(3)});
    [[maybe_unused]] auto b= broadcast_to(a, {2, 3});

    assert(b.shape()[0] == 2);
    assert(b.shape()[1] == 3);
    assert(b.data()[0].to_int64() == 1);
    assert(b.data()[3].to_int64() == 1);
    assert(b.data()[5].to_int64() == 3);

    std::cout << "test_broadcast_ternary passed!" << std::endl;
}

int main() {
    test_ternary_tensor_basic();
    test_ternary_tensor_ops();
    test_reductions();
    test_broadcast_ternary();
    std::cout << "All tensor ternary tests passed!" << std::endl;
    [[maybe_unused]] return 0;
}
