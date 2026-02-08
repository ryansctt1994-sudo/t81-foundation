#include "t81/core/DistributedTensor.hpp"
#include <iostream>
#include <cassert>

using namespace t81;

int main() {
    std::cout << "Starting DistributedTensor test..." << std::endl;

    DistributedT81Tensor<T81Int<81>, 1, 81> t1(0, 2);
    DistributedT81Tensor<T81Int<81>, 1, 81> t2(0, 2);

    t1.local()(0) = T81Int<81>(10);
    t2.local()(0) = T81Int<81>(20);

    t1.add(t2);
    assert(t1.local()(0) == T81Int<81>(30));

    auto sum = t1.reduce_sum();
    assert(sum == T81Int<81>(30));

    // Test MatMul
    DistributedT81Tensor<T81Int<81>, 2, 2, 2> A(0, 1);
    T81Tensor<T81Int<81>, 2, 2, 2> B;
    DistributedT81Tensor<T81Int<81>, 2, 2, 2> C(0, 1);

    A.local()(0, 0) = T81Int<81>(1); A.local()(0, 1) = T81Int<81>(2);
    A.local()(1, 0) = T81Int<81>(3); A.local()(1, 1) = T81Int<81>(4);

    B(0, 0) = T81Int<81>(5); B(0, 1) = T81Int<81>(6);
    B(1, 0) = T81Int<81>(7); B(1, 1) = T81Int<81>(8);

    A.matmul(B, C);

    // [1 2] * [5 6] = [1*5+2*7 1*6+2*8] = [19 22]
    // [3 4]   [7 8]   [3*5+4*7 3*6+4*8]   [43 50]
    assert(C.local()(0, 0) == T81Int<81>(19));
    assert(C.local()(0, 1) == T81Int<81>(22));
    assert(C.local()(1, 0) == T81Int<81>(43));
    assert(C.local()(1, 1) == T81Int<81>(50));

    std::cout << "DistributedTensor test passed!" << std::endl;
    return 0;
}
