#include "t81/core/T81BigInt.hpp"
#include <iostream>
#include <chrono>
#include <vector>

using namespace t81::v1;

int main() {
    T81BigInt a(1234567890123456789LL);
    T81BigInt b(987654321098765432LL);

    // Make them bigger by multiplying repeatedly
    for(int i=0; i<5; ++i) {
        a = a * a;
        b = b * b;
    }

    [[maybe_unused]] auto start= std::chrono::high_resolution_clock::now();

    const int iterations = 10;
    [[maybe_unused]] T81BigInt c;
    for (int i = 0; i < iterations; ++i) {
        c = a * b;
    }

    [[maybe_unused]] auto end= std::chrono::high_resolution_clock::now();
    [[maybe_unused]] std::chrono::duration<double> diff= end - start;

    std::cout << "Time for " << iterations << " multiplications (dense): " << diff.count() << " s" << std::endl;

    [[maybe_unused]] return 0;
}
