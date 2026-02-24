#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <random>
#include <vector>
#include "t81/tritwise/tritwise.hpp"

using namespace t81::tritwise;
using t81::experimental::ComputeTritVector;

// Helper to check vector equality
bool check_vec(const std::vector<int8_t>& a, const std::vector<int8_t>& b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

// Helper to check byte equality
bool check_bytes(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

void test_backend_equivalence() {
  std::cout << "[Equivalence] Testing Backend Equivalence (Scalar vs SWAR vs SIMD)..." << std::endl;
  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(-1, 1);

  // Test a mix of sizes, including small (<64B), medium, and large (>1KB)
  // Also odd sizes to check padding handling.
  std::vector<size_t> sizes = {1, 4, 15, 16, 17, 63, 64, 65, 255, 256, 257, 1024, 4097};

  for (size_t len : sizes) {
    std::vector<int8_t> a_trits(len), b_trits(len);
    for (size_t i = 0; i < len; ++i) {
      a_trits[i] = static_cast<int8_t>(dist(rng));
      b_trits[i] = static_cast<int8_t>(dist(rng));
    }

    auto va = ComputeTritVector::from_trits(a_trits).value();
    auto vb = ComputeTritVector::from_trits(b_trits).value();

    // 1. Scalar Reference (Truth)
    std::vector<int8_t> ref_and(len), ref_or(len), ref_not(len), ref_xor(len);
    for (size_t i = 0; i < len; ++i) {
      ref_and[i] = std::min(a_trits[i], b_trits[i]);
      ref_or[i] = std::max(a_trits[i], b_trits[i]);
      ref_not[i] = -a_trits[i];

      // TXor (Difference)
      int res = a_trits[i] - b_trits[i];
      if (res > 1) res = -1;
      if (res < -1) res = 1;
      ref_xor[i] = (int8_t)res;
    }

    // 2. Library Ops (Dispatches to Best Backend)
    auto res_and = va.t_and(vb).value();
    auto res_or = va.t_or(vb).value();
    auto res_not = va.t_not().value();
    auto res_xor = va.t_xor(vb).value();  // Fallback to LUT

    assert(check_vec(res_and.to_trits().value(), ref_and));
    assert(check_vec(res_or.to_trits().value(), ref_or));
    assert(check_vec(res_not.to_trits().value(), ref_not));
    assert(check_vec(res_xor.to_trits().value(), ref_xor));

    // 3. Explicit Backend Validation (Byte-for-Byte)
    // Verify that SWAR and SIMD (if available) produce IDENTICAL bytes, including padding.

    // Prepare buffers
    size_t byte_len = va.data().size();
    std::vector<uint8_t> swar_not(byte_len), swar_and(byte_len), swar_or(byte_len);
    std::vector<uint8_t> simd_not(byte_len), simd_and(byte_len), simd_or(byte_len);

    const uint8_t* raw_a = va.data().data();
    const uint8_t* raw_b = vb.data().data();

    // Run SWAR
    ComputeTritVector::kernel_not_swar(raw_a, swar_not.data(), byte_len);
    ComputeTritVector::kernel_and_swar(raw_a, raw_b, swar_and.data(), byte_len);
    ComputeTritVector::kernel_or_swar(raw_a, raw_b, swar_or.data(), byte_len);

    // Run SIMD (AVX2)
#if defined(__x86_64__) && defined(__AVX2__)
    ComputeTritVector::kernel_not_avx2(raw_a, simd_not.data(), byte_len);
    ComputeTritVector::kernel_and_avx2(raw_a, raw_b, simd_and.data(), byte_len);
    ComputeTritVector::kernel_or_avx2(raw_a, raw_b, simd_or.data(), byte_len);

    assert(check_bytes(swar_not, simd_not));
    assert(check_bytes(swar_and, simd_and));
    assert(check_bytes(swar_or, simd_or));
#endif

    // Run SIMD (NEON)
#if defined(__aarch64__) && defined(__ARM_NEON)
    ComputeTritVector::kernel_not_neon(raw_a, simd_not.data(), byte_len);
    ComputeTritVector::kernel_and_neon(raw_a, raw_b, simd_and.data(), byte_len);
    ComputeTritVector::kernel_or_neon(raw_a, raw_b, simd_or.data(), byte_len);

    assert(check_bytes(swar_not, simd_not));
    assert(check_bytes(swar_and, simd_and));
    assert(check_bytes(swar_or, simd_or));
#endif
  }
}

void test_txor_fallback_routing() {
  std::cout << "[Compliance] Testing TXor Fallback Routing..." << std::endl;
  // Verify that tritwise_xor matches scalar truth table exactly.
  // We rely on the fact that t_xor calls t_xor_lut.

  // Truth table cases
  struct Case {
    int8_t a, b, expected;
  };
  std::vector<Case> cases = {{-1, -1, 0}, {-1, 0, -1}, {-1, 1, 1}, {0, -1, 1}, {0, 0, 0},
                             {0, 1, -1},  {1, -1, -1}, {1, 0, 1},  {1, 1, 0}};

  for (const auto& c : cases) {
    std::vector<int8_t> va = {c.a};
    std::vector<int8_t> vb = {c.b};

    auto ca = ComputeTritVector::from_trits(va).value();
    auto cb = ComputeTritVector::from_trits(vb).value();

    // In-place API
    assert(tritwise_xor(ca, cb).is_ok());

    auto res = ca.to_trits().value();
    assert(res[0] == c.expected);
  }
}

int main() {
  test_backend_equivalence();
  test_txor_fallback_routing();
  std::cout << "All backend equivalence tests passed." << std::endl;
  return 0;
}
