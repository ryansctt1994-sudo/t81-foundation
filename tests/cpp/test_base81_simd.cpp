#include <iostream>
#include <vector>
#include <cstdlib>
#include <numeric>
#include "t81/simd/base81_digits.hpp"

using namespace t81::simd::base81_digits;

#define EXPECT(cond) \
    if (!(cond)) { \
        std::cerr << "Assertion failed: " << #cond << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::exit(1); \
    }

void test_add_normalize() {
    std::cout << "Testing add + normalize..." << std::endl;
    std::vector<uint8_t> a = {41, 80}; // 1, 40
    std::vector<uint8_t> b = {41, 41}; // 1, 1
    std::vector<uint8_t> raw(2);
    add(a, b, raw);

    EXPECT(raw[0] == 82);
    EXPECT(raw[1] == 121);

    std::vector<uint8_t> out(2);
    normalize_add(raw, out);

    EXPECT(out[0] == 42); // 1+1+40 = 42
    EXPECT(out[1] == 0);  // 40+1+40-40 = 41. Wait.
    // Index 1: raw is 121. 121 - 40 = 81. 81 is carry=1, digit=0.
    // Correct. 40 + 1 = 41 (in balanced). 41 in unbalanced is 81.
    // Since we only have 2 digits, the carry out of index 1 is lost.
    std::cout << "OK" << std::endl;
}

void test_sub_normalize() {
    std::cout << "Testing sub + normalize..." << std::endl;
    // 40 - 41 = -1.
    // -1 + 40 = 39.
    std::vector<uint8_t> a = {40};
    std::vector<uint8_t> b = {41};
    std::vector<int16_t> raw(1);
    sub(a, b, raw);
    EXPECT(raw[0] == -1);

    std::vector<uint8_t> out(1);
    normalize_sub(raw, out);
    EXPECT(out[0] == 39);
    std::cout << "OK" << std::endl;
}

void test_negate() {
    std::cout << "Testing negate..." << std::endl;
    std::vector<uint8_t> in = {0, 40, 80};
    std::vector<uint8_t> out(3);
    negate(in, out);
    EXPECT(out[0] == 80);
    EXPECT(out[1] == 40);
    EXPECT(out[2] == 0);
    std::cout << "OK" << std::endl;
}

void test_compare() {
    std::cout << "Testing compare..." << std::endl;
    std::vector<uint8_t> a = {41, 40}; // 1, 0 -> 1
    std::vector<uint8_t> b = {40, 41}; // 0, 1 -> 81
    EXPECT(compare(a, b) == -1);
    EXPECT(compare(b, a) == 1);
    EXPECT(compare(a, a) == 0);
    std::cout << "OK" << std::endl;
}

int main() {
    test_add_normalize();
    test_sub_normalize();
    test_negate();
    test_compare();
    std::cout << "All SIMD Base-81 tests passed!" << std::endl;
    return 0;
}
