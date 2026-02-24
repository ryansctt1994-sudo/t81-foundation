#undef NDEBUG
#include <cassert>
#include <iostream>
#include <limits>
#include <type_traits>
#include <typeinfo>
#include "t81/types/T81Int.hpp"

using namespace t81;

template <typename T>
void check_success(std::int64_t val) {
  T81Int<42> x(val);
  try {
    T result = x.to_binary<T>();
    // Compare values, but careful with types
    // Cast both to int64_t for comparison if safe, or compare carefully
    // Or check value equality
    if (static_cast<std::int64_t>(result) != val) {
      std::cerr << "FAILED: Value mismatch for val=" << val << " -> " << typeid(T).name() << " got "
                << static_cast<std::int64_t>(result) << "\n";
      std::exit(1);
    }
  } catch (const std::exception& e) {
    std::cerr << "FAILED: Unexpected exception for val=" << val << " -> " << typeid(T).name()
              << ": " << e.what() << "\n";
    std::exit(1);
  }
}

template <typename T>
void check_fail(std::int64_t val) {
  T81Int<42> x(val);
  bool caught = false;
  try {
    [[maybe_unused]] T res = x.to_binary<T>();
  } catch (const std::overflow_error&) {
    caught = true;
  }
  if (!caught) {
    std::cerr << "FAILED: Expected overflow_error for val=" << val << " -> " << typeid(T).name()
              << "\n";
    std::exit(1);
  }
}

int main() {
  std::cout << "Running extended T81Int::to_binary tests...\n";

  // --- Signed Types ---

  // int8_t: -128 .. 127
  check_success<std::int8_t>(0);
  check_success<std::int8_t>(127);
  check_success<std::int8_t>(-128);
  check_fail<std::int8_t>(128);
  check_fail<std::int8_t>(-129);

  // int16_t: -32768 .. 32767
  check_success<std::int16_t>(32767);
  check_success<std::int16_t>(-32768);
  check_fail<std::int16_t>(32768);
  check_fail<std::int16_t>(-32769);

  // int32_t: -2^31 .. 2^31-1
  check_success<std::int32_t>(2147483647);
  check_success<std::int32_t>(-2147483648LL);
  check_fail<std::int32_t>(2147483648LL);
  check_fail<std::int32_t>(-2147483649LL);

  // int64_t: -2^63 .. 2^63-1
  // T81Int now supports full int64 range (trits 0..39 + checked trit 40).
  check_success<std::int64_t>(std::numeric_limits<std::int64_t>::max());
  check_success<std::int64_t>(std::numeric_limits<std::int64_t>::min());

  // --- Unsigned Types ---

  // uint8_t: 0 .. 255
  check_success<std::uint8_t>(0);
  check_success<std::uint8_t>(255);
  check_fail<std::uint8_t>(-1);   // Negative check (crucial fix)
  check_fail<std::uint8_t>(256);  // Overflow check

  // uint16_t: 0 .. 65535
  check_success<std::uint16_t>(0);
  check_success<std::uint16_t>(65535);
  check_fail<std::uint16_t>(-1);
  check_fail<std::uint16_t>(65536);

  // uint32_t: 0 .. 2^32-1
  check_success<std::uint32_t>(0);
  check_success<std::uint32_t>(4294967295U);
  check_fail<std::uint32_t>(-1);
  check_fail<std::uint32_t>(4294967296LL);

  // uint64_t: 0 .. 2^64-1
  check_success<std::uint64_t>(0);
  check_success<std::uint64_t>(
      static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()));
  check_fail<std::uint64_t>(-1);  // The main bug fix case!

  std::cout << "All extended T81Int::to_binary tests passed!\n";
  return 0;
}
