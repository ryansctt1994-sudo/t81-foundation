#include <cstdlib>
#include <iostream>
#include <vector>
#include "t81/codec/base81_packed.hpp"

using namespace t81::codec::base81_packed;

#define EXPECT(cond)                                                                    \
  if (!(cond)) {                                                                        \
    std::cerr << "Assertion failed: " << #cond << " at " << __FILE__ << ":" << __LINE__ \
              << std::endl;                                                             \
    std::exit(1);                                                                       \
  }

void test_pack5_roundtrip() {
  std::cout << "Testing pack5 roundtrip..." << std::endl;
  uint8_t digits[5] = {0, 40, 80, 10, 70};
  uint32_t packed = pack5(digits);
  uint8_t unpacked[5];
  unpack5(packed, unpacked);

  for (int i = 0; i < 5; ++i) {
    EXPECT(digits[i] == unpacked[i]);
  }
  std::cout << "OK" << std::endl;
}

void test_pack_stream() {
  std::cout << "Testing pack_stream..." << std::endl;
  std::vector<uint8_t> digits = {1, 2, 3, 4, 5, 6, 7, 8};
  auto blocks = pack_stream(digits);
  EXPECT(blocks.size() == 2);

  auto unpacked = unpack_stream(blocks, digits.size());
  EXPECT(unpacked.size() == digits.size());
  for (size_t i = 0; i < digits.size(); ++i) {
    EXPECT(unpacked[i] == digits[i]);
  }
  std::cout << "OK" << std::endl;
}

int main() {
  test_pack5_roundtrip();
  test_pack_stream();
  std::cout << "All Base-81 packed tests passed!" << std::endl;
  return 0;
}
