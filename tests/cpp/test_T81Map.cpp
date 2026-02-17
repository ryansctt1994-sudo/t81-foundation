#undef NDEBUG
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "t81/core/T81Entropy.hpp"
#include "t81/core/T81Int.hpp"
#include "t81/core/T81Map.hpp"
#include "t81/core/T81Symbol.hpp"

using namespace t81;

void test_basic() {
  T81Map<T81Symbol, int> map;
  auto k1 = T81Symbol::intern("k1");
  map[k1] = 10;
  assert(map.size() == 1);
  assert(map[k1] == 10);
  std::cout << "test_basic PASSED\n";
}

void test_rehash() {
  T81Map<int, int> map;
  // Default capacity 27. Load factor 0.729.
  // 27 * 0.729 = 19.68 -> 19 elements trigger rehash?
  // Let's insert 100 elements.
  for (int i = 0; i < 100; ++i) {
    map[i] = i * 2;
  }
  assert(map.size() == 100);

  // Verify all
  for (int i = 0; i < 100; ++i) {
    assert(map.contains(i));
    assert(map[i] == i * 2);
  }

  // Verify iterators cover all
  int count = 0;
  for (const auto& [k, v] : map) {
    assert(v == k * 2);
    count++;
  }
  assert(count == 100);

  std::cout << "test_rehash PASSED\n";
}

void test_move_only_value() {
  T81Map<int, std::unique_ptr<int>> map;

  // Insert
  map[1] = std::make_unique<int>(10);
  map[2] = std::make_unique<int>(20);

  assert(map.size() == 2);
  assert(map.contains(1));
  assert(map.contains(2));
  assert(*map.at(1) == 10);

  // Move construction of map
  T81Map<int, std::unique_ptr<int>> moved_map = std::move(map);
  assert(moved_map.size() == 2);

  // Check if source map is actually empty or in a valid state
  // vector move leaves source empty. size_t move is copy for primitives?
  // Actually, T81Map likely uses default move constructor/assignment.
  // Default move ctor performs memberwise move.
  // For primitive `size_t`, move is just copy? No, technically primitives don't have move ctor.
  // So `size_` remains `2` in `map`.
  // But `buckets_` is empty.
  // So `map.size()` returns 2! This is a bug in T81Map if we assume moved-from is empty.
  // But `empty()` checks `size_ == 0`.
  // So `map.empty()` returns false.
  // But `map.buckets_` is empty.
  // Accessing elements would crash?

  // Let's verify if T81Map::size() returns 0 or 2.
  // If it returns 2, we should probably fix T81Map move semantics.
  // For now, let's just assert on the moved-to map.

  assert(*moved_map.at(1) == 10);

  // Moved-from map should be empty
  assert(map.size() == 0);
  assert(map.empty());

  std::cout << "test_move_only_value PASSED\n";
}

void test_erase() {
  T81Map<int, int> map;
  for (int i = 0; i < 10; ++i) {
    map[i] = i;
  }
  assert(map.size() == 10);

  // Erase existing
  assert(map.erase(5) == 1);
  assert(map.size() == 9);
  assert(!map.contains(5));

  // Erase non-existing
  assert(map.erase(99) == 0);
  assert(map.size() == 9);

  // Erase causing displacement handling (hard to force deterministically without knowing hash)
  // But rehash test exercises open addressing.

  std::cout << "test_erase PASSED\n";
}

void test_iterators() {
  T81Map<std::string, int> map;
  map["one"] = 1;
  map["two"] = 2;
  map["three"] = 3;

  int sum = 0;
  for (auto it = map.begin(); it != map.end(); ++it) {
    // Test key() and value() helpers added recently
    sum += it.value();
    assert(it.key().length() > 0);
  }
  assert(sum == 6);

  std::cout << "test_iterators PASSED\n";
}

int main() {
  test_basic();
  test_rehash();
  test_move_only_value();
  test_erase();
  test_iterators();
  std::cout << "All T81Map tests PASSED!\n";
  return 0;
}
