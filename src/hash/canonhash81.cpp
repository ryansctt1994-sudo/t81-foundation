#include "t81/hash/canonhash.hpp"
#include "t81/crypto/sha3.hpp"
#include <algorithm>
#include <span>

namespace t81::hash {

CanonHash81 hash_bytes(const std::vector<std::uint8_t>& data) {
  return hash_bytes(std::span<const std::byte>(reinterpret_cast<const std::byte*>(data.data()), data.size()));
}

CanonHash81 hash_bytes(std::span<const std::byte> data) {
  CanonHash81 h{};
  auto digest = t81::crypto::sha3_512(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(data.data()), data.size()));
  // Truncate to first 32 bytes for 256-bit hash.
  std::copy(digest.begin(), digest.begin() + 32, h.bytes.begin());
  return h;
}

CanonHash81 hash_string(std::string_view s) {
  return hash_bytes(std::span<const std::byte>(reinterpret_cast<const std::byte*>(s.data()), s.size()));
}

}  // namespace t81::hash
