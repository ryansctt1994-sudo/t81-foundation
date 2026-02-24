#include <cassert>
#include <iostream>
#include "t81/types/T81Map.hpp"

using namespace t81;

// Define a key type with predictable hash for testing collisions
struct CollidingKey {
  int id;

  bool operator==(const CollidingKey& other) const { return id == other.id; }
};

namespace std {
template <>
struct hash<CollidingKey> {
  std::size_t operator()(const CollidingKey& /*k*/) const {
    // Force collision: always return 0 (or strictly multiple of bucket count)
    return 0;
  }
};
}  // namespace std

int main() {
  // T81Map starts with size 27.
  T81Map<CollidingKey, int> map;

  // Insert 3 keys that all hash to the same bucket (bucket 0).
  // K1 -> hashes to 0. Lands at 0.
  // K2 -> hashes to 0. Collision at 0. Probes 0+1=1. Lands at 1.
  // K3 -> hashes to 0. Collision at 0. Probes 0+1=1 (collision). Probes 1+3=4. Lands at 4.

  CollidingKey k1{1};
  CollidingKey k2{2};
  CollidingKey k3{3};

  map[k1] = 100;
  map[k2] = 200;
  map[k3] = 300;

  assert(map.size() == 3);
  assert(map.contains(k1));
  assert(map.contains(k2));
  assert(map.contains(k3));

  std::cout << "Initial state: map contains k1, k2, k3.\n";

  // Erase k1 (at bucket 0).
  // The current erase implementation will try to re-insert subsequent elements.
  // It starts at bucket 1 (k2). Moves k2 to bucket 0.
  // Then checks bucket 2. Empty. Stops.
  // k3 remains at bucket 4.
  // But k3's probe sequence was 0 -> 1 -> 4.
  // Now k2 is at 0. So k3 search: 0 (k2, no match) -> 1 (EMPTY!).
  // Fails to find k3.

  std::cout << "Erasing k1...\n";
  map.erase(k1);

  assert(map.size() == 2);
  assert(!map.contains(k1));
  assert(map.contains(k2));

  if (map.contains(k3)) {
    std::cout << "SUCCESS: k3 found after erasing k1.\n";
  } else {
    std::cout << "FAILURE: k3 NOT found after erasing k1! Bug reproduced.\n";
    return 1;
  }

  return 0;
}
