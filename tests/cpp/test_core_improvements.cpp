#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include <algorithm>
#include "t81/core/T81Tensor.hpp"
#include "t81/core/T81Float.hpp"

// Simple assertion macro
#define TEST_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "Assertion failed: " << #cond << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::exit(1); \
        } \
    } while (0)

#define TEST_ASSERT_FLOAT(val, expected) \
    do { \
        if (!((val) == (expected))) { \
            std::cerr << "Assertion failed: " << #val << " == " << #expected << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::cerr << "  Value: " << (val).to_double() << ", Expected: " << (expected).to_double() << std::endl; \
            std::exit(1); \
        } \
    } while (0)


using namespace t81;

// Helper to fill tensor with sequential data
template<typename T, size_t R, size_t... D>
void fill_seq(T81Tensor<T, R, D...>& t) {
    auto s = t.span();
    for (size_t i = 0; i < s.size(); ++i) {
        s[i] = T(static_cast<long long>(i));
    }
}

void test_generic_transpose() {
    std::cout << "Testing generic transpose...\n";

    // Rank 2: (2, 3) -> (3, 2)
    T81Tensor<float81, 2, 2, 3> t2;
    fill_seq(t2);
    // 0 1 2
    // 3 4 5
    auto t2t = transpose(t2);
    static_assert(decltype(t2t)::rank() == 2);
    static_assert(decltype(t2t)::shape()[0] == 3);
    static_assert(decltype(t2t)::shape()[1] == 2);
    // 0 3
    // 1 4
    // 2 5
    TEST_ASSERT_FLOAT(t2t(0, 0), float81(0));
    TEST_ASSERT_FLOAT(t2t(0, 1), float81(3));
    TEST_ASSERT_FLOAT(t2t(1, 0), float81(1));
    TEST_ASSERT_FLOAT(t2t(2, 1), float81(5));

    // Rank 3: (2, 3, 4) -> (4, 3, 2)
    T81Tensor<float81, 3, 2, 3, 4> t3;
    fill_seq(t3);
    // t3(1, 2, 3) is index 1*12 + 2*4 + 3 = 12 + 8 + 3 = 23
    TEST_ASSERT_FLOAT(t3(1, 2, 3), float81(23));

    auto t3t = transpose(t3);
    static_assert(decltype(t3t)::rank() == 3);
    static_assert(decltype(t3t)::shape()[0] == 4);
    static_assert(decltype(t3t)::shape()[2] == 2);
    // t3t(3, 2, 1) should be t3(1, 2, 3) -> 23
    TEST_ASSERT_FLOAT(t3t(3, 2, 1), float81(23));
}

void test_matmul() {
    std::cout << "Testing matmul...\n";

    // (2, 3) x (3, 2) -> (2, 2)
    T81Tensor<float81, 2, 2, 3> A;
    // 1 2 3
    // 4 5 6
    A(0,0)=float81(1); A(0,1)=float81(2); A(0,2)=float81(3);
    A(1,0)=float81(4); A(1,1)=float81(5); A(1,2)=float81(6);

    T81Tensor<float81, 2, 3, 2> B;
    // 7 8
    // 9 1
    // 2 3
    B(0,0)=float81(7); B(0,1)=float81(8);
    B(1,0)=float81(9); B(1,1)=float81(1);
    B(2,0)=float81(2); B(2,1)=float81(3);

    auto C = matmul(A, B);
    static_assert(decltype(C)::shape()[0] == 2);
    static_assert(decltype(C)::shape()[1] == 2);

    // C(0,0) = 1*7 + 2*9 + 3*2 = 7 + 18 + 6 = 31
    // C(0,1) = 1*8 + 2*1 + 3*3 = 8 + 2 + 9 = 19
    // C(1,0) = 4*7 + 5*9 + 6*2 = 28 + 45 + 12 = 85
    // C(1,1) = 4*8 + 5*1 + 6*3 = 32 + 5 + 18 = 55

    TEST_ASSERT_FLOAT(C(0,0), float81(31));
    TEST_ASSERT_FLOAT(C(0,1), float81(19));
    TEST_ASSERT_FLOAT(C(1,0), float81(85));
    TEST_ASSERT_FLOAT(C(1,1), float81(55));
}

void test_matmul_vec() {
    std::cout << "Testing matmul vec...\n";
    // (2, 3) x (3) -> (2)
    T81Tensor<float81, 2, 2, 3> A;
    A(0,0)=float81(1); A(0,1)=float81(2); A(0,2)=float81(3);
    A(1,0)=float81(4); A(1,1)=float81(5); A(1,2)=float81(6);

    T81Tensor<float81, 1, 3> v;
    v(0)=float81(1); v(1)=float81(2); v(2)=float81(3);

    auto r = matmul(A, v);
    static_assert(decltype(r)::rank() == 1);
    static_assert(decltype(r)::shape()[0] == 2);

    // r(0) = 1*1 + 2*2 + 3*3 = 1 + 4 + 9 = 14
    // r(1) = 4*1 + 5*2 + 6*3 = 4 + 10 + 18 = 32

    TEST_ASSERT_FLOAT(r(0), float81(14));
    TEST_ASSERT_FLOAT(r(1), float81(32));
}

void test_scalar_broadcast() {
    std::cout << "Testing scalar broadcast...\n";
    T81Tensor<float81, 1, 3> t;
    t(0)=float81(10); t(1)=float81(20); t(2)=float81(30);

    auto t2 = t + float81(5);
    TEST_ASSERT_FLOAT(t2(0), float81(15));
    TEST_ASSERT_FLOAT(t2(1), float81(25));
    TEST_ASSERT_FLOAT(t2(2), float81(35));

    auto t3 = float81(2) * t;
    TEST_ASSERT_FLOAT(t3(0), float81(20));
    TEST_ASSERT_FLOAT(t3(1), float81(40));
    TEST_ASSERT_FLOAT(t3(2), float81(60));

    auto t4 = t / float81(2);
    TEST_ASSERT_FLOAT(t4(0), float81(5));
    TEST_ASSERT_FLOAT(t4(1), float81(10));
    TEST_ASSERT_FLOAT(t4(2), float81(15));
}

void test_reductions() {
    std::cout << "Testing reductions...\n";
    T81Tensor<float81, 1, 5> t;
    t(0)=float81(1); t(1)=float81(2); t(2)=float81(3); t(3)=float81(4); t(4)=float81(5);

    auto sum = reduce_sum(t);
    TEST_ASSERT_FLOAT(sum, float81(15));

    auto mean = reduce_mean(t);
    // 15 / 5 = 3
    TEST_ASSERT_FLOAT(mean, float81(3));

    auto min = reduce_min(t);
    TEST_ASSERT_FLOAT(min, float81(1));

    auto max = reduce_max(t);
    TEST_ASSERT_FLOAT(max, float81(5));
}

void test_comparison_bug() {
     // Verify T81Float comparison works for negative numbers
     float81 a(-10);
     float81 b(-5);
     // -10 < -5.
     TEST_ASSERT(a < b);
}

int main() {
    test_comparison_bug();
    test_generic_transpose();
    test_matmul();
    test_matmul_vec();
    test_scalar_broadcast();
    test_reductions();
    std::cout << "All core improvement tests passed!\n";
    return 0;
}
