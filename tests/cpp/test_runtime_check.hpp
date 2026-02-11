#pragma once

#include <cstdlib>
#include <iostream>

inline void t81_test_check_impl(bool condition, const char* expr, const char* file, int line) {
  if (condition) {
    return;
  }
  std::cerr << "test check failed: (" << expr << ") at " << file << ":" << line << "\n";
  std::exit(1);
}

#define T81_TEST_CHECK(expr) t81_test_check_impl(static_cast<bool>(expr), #expr, __FILE__, __LINE__)

