#include <iostream>
#include <vector>
#include <stdexcept>
#include "t81/codec/trit_packing.hpp"

using namespace t81;
using namespace t81::codec::trit_packing;

#define EXPECT(cond) if (!(cond)) { \
    std::cerr << "Test failed: " << #cond << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
    std::exit(1); \
}

void test_pt5_roundtrip() {
    std::vector<Trit> trits = {Trit::P, Trit::Z, Trit::N, Trit::P, Trit::P}; // + 0 - + +
    // (1+1) + 3*(0+1) + 9*(-1+1) + 27*(1+1) + 81*(1+1) = 2 + 3 + 0 + 54 + 162 = 221
    auto packed_res = pack_pt5(trits);
    EXPECT(packed_res.is_ok());
    std::vector<uint8_t> packed = packed_res.value();
    EXPECT(packed.size() == 1);
    EXPECT(packed[0] == 221);

    auto unpacked_res = unpack_pt5(packed, 5);
    EXPECT(unpacked_res.is_ok());
    EXPECT(unpacked_res.value() == trits);

    // Test padding
    std::vector<Trit> trits2 = {Trit::P, Trit::Z};
    auto packed2_res = pack_pt5(trits2);
    EXPECT(packed2_res.is_ok());
    std::vector<uint8_t> packed2 = packed2_res.value();
    EXPECT(packed2[0] == (2 + 3*1 + 9*1 + 27*1 + 81*1)); // 2+3+9+27+81 = 122 (padded with Z)
    auto unpacked2_res = unpack_pt5(packed2, 2);
    EXPECT(unpacked2_res.is_ok());
    EXPECT(unpacked2_res.value() == trits2);
}

void test_base81_roundtrip() {
    std::vector<Trit> trits = {Trit::P, Trit::Z, Trit::N, Trit::P}; // + 0 - +
    // (1+1) + 3*(0+1) + 9*(-1+1) + 27*(1+1) = 2 + 3 + 0 + 54 = 59
    auto packed_res = pack_base81(trits);
    EXPECT(packed_res.is_ok());
    std::vector<uint8_t> packed = packed_res.value();
    EXPECT(packed.size() == 1);
    EXPECT(packed[0] == 59);

    auto unpacked_res = unpack_base81(packed, 4);
    EXPECT(unpacked_res.is_ok());
    EXPECT(unpacked_res.value() == trits);
}

void test_direct_conversion() {
    // 20 trits (divisible by 4 and 5)
    std::vector<Trit> trits;
    for (int i = 0; i < 20; ++i) {
        trits.push_back(static_cast<Trit>((i % 3) - 1));
    }

    auto pt5 = pack_pt5(trits).value();
    auto b81 = pack_base81(trits).value();

    auto converted_b81_res = pt5_to_b81(pt5, 20);
    EXPECT(converted_b81_res.is_ok());
    EXPECT(converted_b81_res.value() == b81);

    auto converted_pt5_res = b81_to_pt5(b81, 20);
    EXPECT(converted_pt5_res.is_ok());
    EXPECT(converted_pt5_res.value() == pt5);

    // Partial groups
    size_t trit_count = 7;
    std::vector<Trit> trits_partial = {Trit::P, Trit::N, Trit::Z, Trit::P, Trit::P, Trit::Z, Trit::N};
    auto pt5_p = pack_pt5(trits_partial).value();
    auto b81_p = pack_base81(trits_partial).value();

    auto res1 = pt5_to_b81(pt5_p, trit_count);
    EXPECT(res1.is_ok());
    EXPECT(res1.value() == b81_p);

    auto res2 = b81_to_pt5(b81_p, trit_count);
    EXPECT(res2.is_ok());
    EXPECT(res2.value() == pt5_p);
}

void test_invalid_inputs() {
    // Invalid PT-5 byte
    std::vector<uint8_t> bad_pt5 = {243};
    EXPECT(unpack_pt5(bad_pt5, 5).is_err());
    EXPECT(pt5_to_b81(bad_pt5, 5).is_err());

    // Invalid Base-81 digit
    std::vector<uint8_t> bad_b81 = {81};
    EXPECT(unpack_base81(bad_b81, 4).is_err());
    EXPECT(b81_to_pt5(bad_b81, 4).is_err());

    // Insufficient data
    std::vector<uint8_t> short_pt5 = {121};
    EXPECT(unpack_pt5(short_pt5, 6).is_err());
}

void test_string_conversion() {
    std::vector<uint8_t> digits = {0, 10, 80};
    std::string s = b81_digits_to_string(digits);
    // "0" is 0, "A" is 10, "Γ" is 80
    EXPECT(s == "0AΓ");

    auto back_res = string_to_b81_digits(s);
    EXPECT(back_res.is_ok());
    EXPECT(back_res.value() == digits);

    EXPECT(string_to_b81_digits("invalid!").is_err());
}

int main() {
    test_pt5_roundtrip();
    test_base81_roundtrip();
    test_direct_conversion();
    test_invalid_inputs();
    test_string_conversion();

    std::cout << "trit_packing_test ok" << std::endl;
    return 0;
}
