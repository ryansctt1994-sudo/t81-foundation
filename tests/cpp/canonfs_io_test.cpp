#include <cassert>
#include <cstring>
#include <iostream>
#include "t81/canonfs.hpp"
#include "t81/canonfs_io.hpp"

int main() {
  using namespace t81;

  // Build a CanonRef from a valid Base-81 hash (example from hash_string("test"))
  [[maybe_unused]] std::string valid_b81= hash::hash_string("test").to_string();
  [[maybe_unused]] CanonHash81 h= CanonHash81::from_string(valid_b81);
  [[maybe_unused]] CanonRef ref= CanonRef::make(h, CANON_PERM_READ | CANON_PERM_WRITE, 0x1122334455667788ull);

  // Encode → bytes[99]
  uint8_t buf[t81::canonfs_io::kWireSize];
  t81::canonfs_io::encode_ref(ref, buf);

  // Decode → back
  [[maybe_unused]] CanonRef got= t81::canonfs_io::decode_ref(buf);

  // Check roundtrip
  assert(got.permissions == (CANON_PERM_READ | CANON_PERM_WRITE));
  assert(got.expires_at == 0x1122334455667788ull);
  assert(got.target.to_string() == h.to_string());

  // Permission helper
  assert(t81::canonfs_io::permissions_allow(got.permissions, CANON_PERM_READ));
  assert(!t81::canonfs_io::permissions_allow(got.permissions, CANON_PERM_ADMIN));

  std::cout << "canonfs_io ok\n";
  return 0;
}
