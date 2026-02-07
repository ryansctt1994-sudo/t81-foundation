#include <cassert>
#include <string>
#include <vector>
#include <t81/canonfs/canon_driver.hpp>
#include <t81/hash/canonhash.hpp>

int main() {
  using namespace t81::canonfs;

  [[maybe_unused]] auto driver= make_in_memory_driver();
  const std::string payload = "hello-canonfs";
  [[maybe_unused]] auto ref_res= driver->write_object(ObjectType::Blob, std::span<const std::byte>(reinterpret_cast<const std::byte*>(payload.data()), payload.size()));
  assert(ref_res.has_value());
  [[maybe_unused]] auto ref= ref_res.value();
  [[maybe_unused]] auto read= driver->read_object_bytes(ref);
  assert(read.has_value());
  std::string back(reinterpret_cast<const char*>(read.value().data()), read.value().size());
  assert(back == payload);

  // Rewriting the same bytes should yield the same hash (content addressable)
  [[maybe_unused]] auto ref_res2= driver->write_object(ObjectType::Blob, std::span<const std::byte>(reinterpret_cast<const std::byte*>(payload.data()), payload.size()));
  assert(ref_res2.has_value());
  assert(ref_res2.value().hash == ref.hash);

  // Capability enforcement: publish only for ref, then access succeeds; unknown should fail.
  CapabilityGrant grant{ref, {"userA", "pk-userA"}, CANON_PERM_READ | CANON_PERM_WRITE};
  [[maybe_unused]] auto cap_res= driver->publish_capability(grant);
  assert(cap_res.has_value());
  [[maybe_unused]] auto read_authed= driver->read_object_bytes(ref);
  assert(read_authed.has_value());

  CanonRef no_cap{ref.hash}; // same hash, but cap exists; still allowed because capability was published.
  [[maybe_unused]] auto read_again= driver->read_object_bytes(no_cap);
  assert(read_again.has_value());

  CanonRef bogus{CanonHash{t81::hash::hash_string("bogus")}};
  [[maybe_unused]] auto miss_cap= driver->read_object_bytes(bogus);
  assert(!miss_cap.has_value());
  assert(miss_cap.error() == Error::CapabilityError);

  [[maybe_unused]] return 0;
}
