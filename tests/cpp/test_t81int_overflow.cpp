#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>
#include "t81/types/T81Int.hpp"

using namespace t81;

// Helper to check if a function throws specific exception
template <typename E, typename F>
bool throws_overflow(F&& f, std::string& out_msg) {
  try {
    f();
    return false;
  } catch (const E& e) {
    out_msg = e.what();
    return true;
  } catch (...) {
    out_msg = "Unknown exception";
    return false;
  }
}

int main() {
  std::cout << "Running T81Int overflow tests...\n";

  bool all_passed = true;

  auto test_index39_success = [&]() {
    // Index 39 is now safe (kPow3AccumTrits = 40, so 0..39 are safe).
    T81Int<45> val;
    val[39] = Trit::P;  // 3^39
    try {
      std::int64_t v = val.to_int64();
      if (v > 0) {
        std::cout << "PASS: Index 39 conversion succeeded. Value: " << v << "\n";
        return true;
      } else {
        std::cerr << "FAIL: Index 39 value non-positive\n";
        return false;
      }
    } catch (const std::exception& e) {
      std::cerr << "FAIL: Index 39 conversion threw: " << e.what() << "\n";
      return false;
    }
  };

  auto test_index40_success = [&]() {
    // Use INT64_MAX which uses index 40 but fits.
    // INT64_MAX = 9223372036854775807
    T81Int<45> val(std::numeric_limits<std::int64_t>::max());
    try {
      std::int64_t v = val.to_int64();
      if (v == std::numeric_limits<std::int64_t>::max()) {
        std::cout << "PASS: Index 40 valid value (INT64_MAX) succeeded.\n";
        return true;
      } else {
        std::cerr << "FAIL: Index 40 value mismatch\n";
        return false;
      }
    } catch (const std::exception& e) {
      std::cerr << "FAIL: Index 40 valid value threw: " << e.what() << "\n";
      return false;
    }
  };

  auto test_index40_overflow = [&]() {
    // 3^40 alone overflows int64
    T81Int<45> val;
    val[40] = Trit::P;

    std::string msg;
    if (throws_overflow<std::overflow_error>([&] { [[maybe_unused]] auto _ = val.to_int64(); },
                                             msg)) {
      if (msg.find("overflow (positive)") != std::string::npos) {
        std::cout << "PASS: Index 40 overflow (positive) detected.\n";
        return true;
      } else {
        std::cerr << "FAIL: Index 40 overflow threw wrong message: " << msg << "\n";
        return false;
      }
    }
    std::cerr << "FAIL: Index 40 overflow did not throw\n";
    return false;
  };

  auto test_index41_throw = [&]() {
    // Index 41 is out of range entirely (must be zero)
    T81Int<45> val;
    val[41] = Trit::P;
    std::string msg;
    if (throws_overflow<std::overflow_error>([&] { [[maybe_unused]] auto _ = val.to_int64(); },
                                             msg)) {
      if (msg.find("value out of range") != std::string::npos) {
        std::cout << "PASS: Index 41 threw 'value out of range' as expected.\n";
        return true;
      }
      std::cerr << "FAIL: Index 41 threw wrong message: " << msg << "\n";
      return false;
    }
    std::cerr << "FAIL: Index 41 did not throw\n";
    return false;
  };

  auto test_huge_value = [&]() {
    // Definitely overflowing value
    T81Int<60> huge_val;
    huge_val[55] = Trit::P;  // 3^55
    std::string msg;
    if (throws_overflow<std::overflow_error>([&] { [[maybe_unused]] auto _ = huge_val.to_int64(); },
                                             msg)) {
      std::cout << "PASS: Huge value threw as expected.\n";
      return true;
    } else {
      std::cerr << "FAIL: Huge value did not throw\n";
      return false;
    }
  };

  // Run tests
  if (!test_index39_success()) all_passed = false;
  if (!test_index40_success()) all_passed = false;
  if (!test_index40_overflow()) all_passed = false;
  if (!test_index41_throw()) all_passed = false;
  if (!test_huge_value()) all_passed = false;

  if (all_passed) {
    std::cout << "All T81Int overflow tests passed!\n";
    return 0;
  } else {
    std::cerr << "Some tests failed!\n";
    return 1;
  }
}
