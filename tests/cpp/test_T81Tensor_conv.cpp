#include "t81/core/T81Tensor.hpp"
#include <iostream>
#include <cstdlib>

using namespace t81;

// Use T81Int<81> for simplicity
using TInt = T81Int<81>;

void check(bool cond, const char* msg) {
    if (!cond) {
        std::cerr << "Check failed: " << msg << "\n";
        std::exit(1);
    }
}

void test_conv2d() {
    T81Tensor<TInt, 4, 1, 1, 3, 3> input;
    for(int i=0; i<9; ++i) input.data[i] = TInt(i+1);

    T81Tensor<TInt, 4, 1, 1, 2, 2> weight;
    weight(0,0,0,0) = TInt(1); weight(0,0,0,1) = TInt(0);
    weight(0,0,1,0) = TInt(0); weight(0,0,1,1) = TInt(1);

    auto out = conv2d(input, weight);

    check(out(0,0,0,0) == TInt(6), "conv out(0,0,0,0)");
    check(out(0,0,0,1) == TInt(8), "conv out(0,0,0,1)");
    check(out(0,0,1,0) == TInt(12), "conv out(0,0,1,0)");
    check(out(0,0,1,1) == TInt(14), "conv out(0,0,1,1)");

    std::cout << "test_conv2d passed\n";
}

void test_max_pool2d() {
    T81Tensor<TInt, 4, 1, 1, 4, 4> input;
    for(int i=0; i<16; ++i) input.data[i] = TInt(i+1);

    auto out = max_pool2d<2, 2, 2, 2>(input);

    check(out(0,0,0,0) == TInt(6), "pool out(0,0,0,0)");
    check(out(0,0,0,1) == TInt(8), "pool out(0,0,0,1)");
    check(out(0,0,1,0) == TInt(14), "pool out(0,0,1,0)");
    check(out(0,0,1,1) == TInt(16), "pool out(0,0,1,1)");

    std::cout << "test_max_pool2d passed\n";
}

int main() {
    test_conv2d();
    test_max_pool2d();
    return 0;
}
