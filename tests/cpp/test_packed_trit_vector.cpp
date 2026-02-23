#include <vector>
#include <iostream>
#include <cassert>
#include <random>
#include <algorithm>
#include "t81/experimental/packed_trit_vector.hpp"

using namespace t81::experimental;
using namespace t81;

// Helper to check vector equality
bool check_vec(const std::vector<int8_t>& a, const std::vector<int8_t>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

// Phase 1 Tests
void test_phase1_roundtrip() {
    std::cout << "[Phase 1] Testing Roundtrip..." << std::endl;
    std::vector<int8_t> trits = {-1, 0, 1, 1, 0, -1, 0, 1};
    auto vec_res = PackedTritVector::from_trits(trits);
    assert(!vec_res.is_err());
    auto vec = vec_res.value();
    assert(vec.size() == trits.size());

    auto unpacked_res = vec.to_trits();
    assert(!unpacked_res.is_err());
    auto unpacked = unpacked_res.value();
    assert(check_vec(unpacked, trits));
}

void test_scalar_logic_basis() {
    std::cout << "[Scalar] Testing Logic Basis..." << std::endl;
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

    // TXor: a - b (wrapped) - Canonical TISC semantics
    assert(PackedTritVector::scalar_xor(-1, -1) == 0);
    assert(PackedTritVector::scalar_xor(-1, 0) == -1);
    assert(PackedTritVector::scalar_xor(-1, 1) == 1); // -1 - 1 = -2 -> 1
    assert(PackedTritVector::scalar_xor(0, -1) == 1); // 0 - -1 = 1
    assert(PackedTritVector::scalar_xor(0, 0) == 0);
    assert(PackedTritVector::scalar_xor(0, 1) == -1);
    assert(PackedTritVector::scalar_xor(1, -1) == -1); // 1 - -1 = 2 -> -1
    assert(PackedTritVector::scalar_xor(1, 0) == 1);
    assert(PackedTritVector::scalar_xor(1, 1) == 0);
}

// Explicit TXor Truth Table Guard
void test_txor_truth_table() {
    std::cout << "[Guard] Testing TXor Truth Table (9 cases)..." << std::endl;
    struct Case { int8_t a; int8_t b; int8_t expected; };
    std::vector<Case> cases = {
        {-1, -1, 0}, {-1, 0, -1}, {-1, 1, 1},
        {0, -1, 1},  {0, 0, 0},   {0, 1, -1},
        {1, -1, -1}, {1, 0, 1},   {1, 1, 0}
    };

    for (const auto& c : cases) {
        // 1. Scalar check
        assert(PackedTritVector::scalar_xor(c.a, c.b) == c.expected);

        // 2. Phase 1 check
        auto p1_a = PackedTritVector::from_trits({c.a}).value();
        auto p1_b = PackedTritVector::from_trits({c.b}).value();
        auto p1_res = p1_a.t_xor(p1_b).value();
        assert(p1_res.to_trits().value()[0] == c.expected);

        // 3. Phase 2A check
        auto p2_a = ComputeTritVector::from_trits({c.a}).value();
        auto p2_b = ComputeTritVector::from_trits({c.b}).value();
        auto p2_res = p2_a.t_xor(p2_b).value();
        assert(p2_res.to_trits().value()[0] == c.expected);
    }
}

// Phase 2A Tests
void test_phase2a_roundtrip() {
    std::cout << "[Phase 2A] Testing Roundtrip..." << std::endl;
    std::vector<int8_t> trits = {-1, 0, 1, 1, 0, -1, 0, 1};
    auto vec_res = ComputeTritVector::from_trits(trits);
    assert(!vec_res.is_err());
    auto vec = vec_res.value();
    assert(vec.size() == trits.size());

    // Check data size (2 bits per trit -> 8 trits fits in 2 bytes)
    assert(vec.data().size() == 2);

    auto unpacked_res = vec.to_trits();
    assert(!unpacked_res.is_err());
    auto unpacked = unpacked_res.value();
    assert(check_vec(unpacked, trits));
}

void test_phase2a_conversion() {
    std::cout << "[Phase 2A] Testing Conversion from Phase 1..." << std::endl;
    std::vector<int8_t> trits = {-1, 0, 1};
    auto p1 = PackedTritVector::from_trits(trits).value();

    auto p2_res = ComputeTritVector::from_phase1(p1);
    assert(!p2_res.is_err());
    auto p2 = p2_res.value();

    assert(check_vec(p2.to_trits().value(), trits));
}

void test_cross_representation_consistency() {
    std::cout << "[Cross-Rep] Testing Consistency..." << std::endl;
    std::vector<int8_t> t1 = {-1, 0, 1, 1, -1};
    std::vector<int8_t> t2 = {1, -1, 0, 1, -1};

    // Scalar Reference
    std::vector<int8_t> ref_not, ref_and, ref_or, ref_xor;
    for(size_t i=0; i<t1.size(); ++i) {
        ref_not.push_back(PackedTritVector::scalar_not(t1[i]));
        ref_and.push_back(PackedTritVector::scalar_and(t1[i], t2[i]));
        ref_or.push_back(PackedTritVector::scalar_or(t1[i], t2[i]));
        ref_xor.push_back(PackedTritVector::scalar_xor(t1[i], t2[i]));
    }

    // Phase 1
    auto p1_v1 = PackedTritVector::from_trits(t1).value();
    auto p1_v2 = PackedTritVector::from_trits(t2).value();
    assert(check_vec(p1_v1.t_not().value().to_trits().value(), ref_not));
    assert(check_vec(p1_v1.t_and(p1_v2).value().to_trits().value(), ref_and));
    assert(check_vec(p1_v1.t_or(p1_v2).value().to_trits().value(), ref_or));
    assert(check_vec(p1_v1.t_xor(p1_v2).value().to_trits().value(), ref_xor));

    // Phase 2A
    auto p2_v1 = ComputeTritVector::from_trits(t1).value();
    auto p2_v2 = ComputeTritVector::from_trits(t2).value();
    assert(check_vec(p2_v1.t_not().value().to_trits().value(), ref_not));
    assert(check_vec(p2_v1.t_and(p2_v2).value().to_trits().value(), ref_and));
    assert(check_vec(p2_v1.t_or(p2_v2).value().to_trits().value(), ref_or));
    assert(check_vec(p2_v1.t_xor(p2_v2).value().to_trits().value(), ref_xor));
}

void test_randomized_determinism() {
    std::cout << "[Random] Testing Deterministic Randomized Vectors..." << std::endl;
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-1, 1);

    for (int len : {0, 1, 2, 4, 5, 6, 16, 64, 100}) {
        std::vector<int8_t> t1_data;
        std::vector<int8_t> t2_data;
        for (int i = 0; i < len; ++i) {
            t1_data.push_back(static_cast<int8_t>(dist(rng)));
            t2_data.push_back(static_cast<int8_t>(dist(rng)));
        }

        // Phase 1
        auto p1_v1 = PackedTritVector::from_trits(t1_data).value();
        auto p1_v2 = PackedTritVector::from_trits(t2_data).value();

        // Phase 2A
        auto p2_v1 = ComputeTritVector::from_trits(t1_data).value();
        auto p2_v2 = ComputeTritVector::from_trits(t2_data).value();

        // Check equivalence for all ops
        // Not
        assert(check_vec(p1_v1.t_not().value().to_trits().value(),
                         p2_v1.t_not().value().to_trits().value()));

        // And
        assert(check_vec(p1_v1.t_and(p1_v2).value().to_trits().value(),
                         p2_v1.t_and(p2_v2).value().to_trits().value()));

        // Or
        assert(check_vec(p1_v1.t_or(p1_v2).value().to_trits().value(),
                         p2_v1.t_or(p2_v2).value().to_trits().value()));

        // Xor
        assert(check_vec(p1_v1.t_xor(p1_v2).value().to_trits().value(),
                         p2_v1.t_xor(p2_v2).value().to_trits().value()));
    }
}

void test_errors() {
    std::cout << "[Errors] Testing Error Conditions..." << std::endl;

    // Invalid trit
    std::vector<int8_t> bad = {2};
    assert(PackedTritVector::from_trits(bad).is_err());
    assert(ComputeTritVector::from_trits(bad).is_err());

    // Length mismatch
    std::vector<int8_t> t1 = {0};
    std::vector<int8_t> t2 = {0, 0};

    auto p1_v1 = PackedTritVector::from_trits(t1).value();
    auto p1_v2 = PackedTritVector::from_trits(t2).value();
    assert(p1_v1.t_and(p1_v2).is_err());

    auto p2_v1 = ComputeTritVector::from_trits(t1).value();
    auto p2_v2 = ComputeTritVector::from_trits(t2).value();
    assert(p2_v1.t_and(p2_v2).is_err());
}

int main() {
    test_phase1_roundtrip();
    test_scalar_logic_basis();
    test_txor_truth_table();
    test_phase2a_roundtrip();
    test_phase2a_conversion();
    test_cross_representation_consistency();
    test_randomized_determinism();
    test_errors();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
