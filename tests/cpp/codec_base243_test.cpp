#include <iostream>
#include <string>
#include <vector>
#include "t81/codec/base243.hpp"
#include "t81/bigint.hpp"

int main() {
  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "codec_base243_test failure: " << msg << "\n";
      return false;
    }
    return true;
  };

  using namespace t81::codec;

  // --- bytes <-> digits roundtrip (big-endian order preserved) ---
  {
    std::vector<uint8_t> bytes = {0xFF}; // 255 = 1*243 + 12 -> digits {1,12}
    [[maybe_unused]] auto digits= Base243::encode_bytes_be(bytes);
    if (!expect((digits == std::vector<digit_t>{1, 12}), "single-byte encode mismatch")) return 1;
    [[maybe_unused]] auto round= Base243::decode_bytes_be(digits);
    if (!expect(round == bytes, "single-byte roundtrip mismatch")) return 1;
  }

  {
    std::vector<uint8_t> bytes = {0x01, 0x00}; // 256 = 1*243 + 13 -> digits {1,13}
    [[maybe_unused]] auto digits= Base243::encode_bytes_be(bytes);
    if (!expect((digits == std::vector<digit_t>{1, 13}), "multi-byte encode mismatch")) return 1;
    [[maybe_unused]] auto round= Base243::decode_bytes_be(digits);
    if (!expect(round == bytes, "multi-byte roundtrip mismatch")) return 1;
  }

  // --- ASCII helpers (stubbed mapping) ---
  {
    [[maybe_unused]] std::string s= "T81-base243";
    [[maybe_unused]] auto digits= Base243::encode_ascii(s);
    if (!expect(digits.size() == s.size(), "ASCII encode size mismatch")) return 1;
    // decode is lossy inverse but valid for <=242
    [[maybe_unused]] auto s2= Base243::decode_ascii(digits);
    // With pure ASCII, digits are <=127 so roundtrip equals original
    if (!expect(s2 == s, "ASCII roundtrip mismatch")) return 1;
  }

  // --- decode guard: digit out of range should throw ---
  {
    [[maybe_unused]] bool threw= false;
    try {
      std::vector<digit_t> bad = {0, 1, static_cast<digit_t>(244)}; // 244 >= 243
      (void)Base243::decode_bytes_be(bad);
    } catch (const std::invalid_argument&) {
      threw = true;
    }
    if (!expect(threw, "out-of-range digit did not throw")) return 1;
  }

  // --- bigint roundtrip ---
  {
    t81:: T81BigInt a = t81::T81BigInt::from_i64(123456);
    [[maybe_unused]] auto s= Base243::encode_bigint(a);
    [[maybe_unused]] t81::T81BigInt b;
    [[maybe_unused]] bool ok= Base243::decode_bigint(s, b);
    if (!expect(ok, "positive bigint decode failed")) return 1;
    if (!expect(a == b, "positive bigint roundtrip mismatch")) return 1;

    t81:: T81BigInt neg = t81::T81BigInt::from_i64(-999);
    [[maybe_unused]] auto sn= Base243::encode_bigint(neg);
    [[maybe_unused]] t81::T81BigInt back;
    ok = Base243::decode_bigint(sn, back);
    if (!expect(ok, "negative bigint decode failed")) return 1;
    if (!expect(neg == back, "negative bigint roundtrip mismatch")) return 1;
  }

  std::cout << "codec_base243 ok\n";
  return 0;
}
