#include <vector>
#include <iostream>
#include <cassert>
#include <random>
#include "t81/experimental/packed_trit_vector.hpp"

using namespace t81::experimental;
using namespace t81;

void test_roundtrip() {
    std::cout << "Testing Roundtrip..." << std::endl;
    std::vector<int8_t> trits = {-1, 0, 1, 1, 0, -1, 0, 1};
    auto vec_res = PackedTritVector::from_trits(trits);
    if (vec_res.is_err()) {
        std::cerr << "Roundtrip failed to create: " << vec_res.error().message.str() << std::endl;
        assert(false);
    }
    auto vec = vec_res.value();
    assert(vec.size() == trits.size());

    auto unpacked_res = vec.to_trits();
    if (unpacked_res.is_err()) {
        std::cerr << "Roundtrip failed to unpack: " << unpacked_res.error().message.str() << std::endl;
        assert(false);
    }
    auto unpacked = unpacked_res.value();
    assert(unpacked.size() == trits.size());
    for (size_t i = 0; i < trits.size(); ++i) {
        assert(unpacked[i] == trits[i]);
    }
    std::cout << "Roundtrip OK" << std::endl;
}

void test_scalar_ops() {
    std::cout << "Testing Scalar Ops..." << std::endl;
    // TNot
    assert(PackedTritVector::scalar_not(1) == -1);
    assert(PackedTritVector::scalar_not(0) == 0);
    assert(PackedTritVector::scalar_not(-1) == 1);

    // TAnd: min
    assert(PackedTritVector::scalar_and(-1, -1) == -1);
    assert(PackedTritVector::scalar_and(-1, 0) == -1);
    assert(PackedTritVector::scalar_and(-1, 1) == -1);
    assert(PackedTritVector::scalar_and(0, 0) == 0);
    assert(PackedTritVector::scalar_and(0, 1) == 0);
    assert(PackedTritVector::scalar_and(1, 1) == 1);

    // TOr: max
    assert(PackedTritVector::scalar_or(-1, -1) == -1);
    assert(PackedTritVector::scalar_or(-1, 0) == 0);
    assert(PackedTritVector::scalar_or(-1, 1) == 1);
    assert(PackedTritVector::scalar_or(0, 0) == 0);
    assert(PackedTritVector::scalar_or(0, 1) == 1);
    assert(PackedTritVector::scalar_or(1, 1) == 1);

    // TXor: a - b (wrapped)
    assert(PackedTritVector::scalar_xor(-1, -1) == 0);
    assert(PackedTritVector::scalar_xor(-1, 0) == -1);
    assert(PackedTritVector::scalar_xor(-1, 1) == 1); // -1 - 1 = -2 -> 1
    assert(PackedTritVector::scalar_xor(0, -1) == 1); // 0 - -1 = 1
    assert(PackedTritVector::scalar_xor(0, 0) == 0);
    assert(PackedTritVector::scalar_xor(0, 1) == -1);
    assert(PackedTritVector::scalar_xor(1, -1) == -1); // 1 - -1 = 2 -> -1
    assert(PackedTritVector::scalar_xor(1, 0) == 1);
    assert(PackedTritVector::scalar_xor(1, 1) == 0);

    std::cout << "Scalar Ops OK" << std::endl;
}

void test_packed_ops() {
    std::cout << "Testing Packed Ops..." << std::endl;
    std::vector<int8_t> t1 = {-1, 0, 1, 1, -1};
    std::vector<int8_t> t2 = {1, -1, 0, 1, -1};
    auto v1 = PackedTritVector::from_trits(t1).value();
    auto v2 = PackedTritVector::from_trits(t2).value();

    // Not
    auto not_v1 = v1.t_not().value().to_trits().value();
    for (size_t i = 0; i < t1.size(); ++i) {
        assert(not_v1[i] == PackedTritVector::scalar_not(t1[i]));
    }

    // And
    auto and_v = v1.t_and(v2).value().to_trits().value();
    for (size_t i = 0; i < t1.size(); ++i) {
        assert(and_v[i] == PackedTritVector::scalar_and(t1[i], t2[i]));
    }

    // Or
    auto or_v = v1.t_or(v2).value().to_trits().value();
    for (size_t i = 0; i < t1.size(); ++i) {
        assert(or_v[i] == PackedTritVector::scalar_or(t1[i], t2[i]));
    }

    // Xor
    auto xor_v = v1.t_xor(v2).value().to_trits().value();
    for (size_t i = 0; i < t1.size(); ++i) {
        assert(xor_v[i] == PackedTritVector::scalar_xor(t1[i], t2[i]));
    }
    std::cout << "Packed Ops OK" << std::endl;
}

void test_errors() {
    std::cout << "Testing Errors..." << std::endl;
    // Invalid trit
    std::vector<int8_t> bad = {2};
    auto res = PackedTritVector::from_trits(bad);
    assert(res.is_err());

    // Length mismatch
    std::vector<int8_t> t1 = {0};
    std::vector<int8_t> t2 = {0, 0};
    auto v1 = PackedTritVector::from_trits(t1).value();
    auto v2 = PackedTritVector::from_trits(t2).value();
    assert(v1.t_and(v2).is_err());
    std::cout << "Errors OK" << std::endl;
}

void test_random() {
    std::cout << "Testing Random Vectors..." << std::endl;
    std::mt19937 rng(42); // Deterministic seed
    std::uniform_int_distribution<int> dist(-1, 1);

    for (int len : {0, 1, 2, 4, 5, 6, 16, 64, 100}) {
        std::vector<int8_t> t1_data;
        std::vector<int8_t> t2_data;
        for (int i = 0; i < len; ++i) {
            t1_data.push_back(static_cast<int8_t>(dist(rng)));
            t2_data.push_back(static_cast<int8_t>(dist(rng)));
        }

        auto v1 = PackedTritVector::from_trits(t1_data).value();
        auto v2 = PackedTritVector::from_trits(t2_data).value();

        // Not
        auto not_res = v1.t_not().value().to_trits().value();
        for(int i=0; i<len; ++i) {
            assert(not_res[i] == PackedTritVector::scalar_not(t1_data[i]));
        }

        // And
        auto and_res = v1.t_and(v2).value().to_trits().value();
        for(int i=0; i<len; ++i) {
            assert(and_res[i] == PackedTritVector::scalar_and(t1_data[i], t2_data[i]));
        }

        // Or
        auto or_res = v1.t_or(v2).value().to_trits().value();
        for(int i=0; i<len; ++i) {
            assert(or_res[i] == PackedTritVector::scalar_or(t1_data[i], t2_data[i]));
        }

        // Xor
        auto xor_res = v1.t_xor(v2).value().to_trits().value();
        for(int i=0; i<len; ++i) {
            assert(xor_res[i] == PackedTritVector::scalar_xor(t1_data[i], t2_data[i]));
        }
    }
    std::cout << "Random Vectors OK" << std::endl;
}

int main() {
    test_roundtrip();
    test_scalar_ops();
    test_packed_ops();
    test_errors();
    test_random();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
