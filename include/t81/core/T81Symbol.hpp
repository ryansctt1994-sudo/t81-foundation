/**
 * @file T81Symbol.hpp
 * @brief T81Symbol — eternal, unique, 81-trit identity.
 */
#pragma once

#include "t81/core/T81Int.hpp"

#include <atomic>
#include <compare>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace t81 {

class T81Symbol {
public:
  using Raw = T81Int<81>;
  using id_t = std::uint64_t;

  static constexpr size_t kTrits = 81;

private:
  Raw value_;

  explicit constexpr T81Symbol(Raw v) noexcept : value_(v) {}

public:
  constexpr T81Symbol() noexcept = default;

  static T81Symbol intern(std::string_view name) noexcept;
  static T81Symbol intern(const char* name) noexcept { return intern(std::string_view(name)); }

  static T81Symbol from_id(id_t id) noexcept {
    return T81Symbol(Raw(static_cast<std::int64_t>(id)));
  }

  static constexpr T81Symbol from_raw(Raw r) noexcept { return T81Symbol(r); }

  [[nodiscard]] constexpr Raw raw() const noexcept { return value_; }
  [[nodiscard]] constexpr id_t id() const noexcept { return static_cast<id_t>(value_.to_int64()); }

  [[nodiscard]] constexpr bool is_valid() const noexcept { return !value_.is_zero(); }

  [[nodiscard]] constexpr auto operator<=>(const T81Symbol&) const noexcept = default;
  [[nodiscard]] constexpr bool operator==(const T81Symbol&) const noexcept = default;

  [[nodiscard]] constexpr std::uint64_t hash() const noexcept {
    std::uint64_t h = id();
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccdull;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53ull;
    h ^= h >> 33;
    return h;
  }

  [[nodiscard]] std::string to_string() const noexcept;

  // P2: Canonical serialization (name based)
  [[nodiscard]] std::string serialize_canonical() const { return to_string(); }
};

// Internal implementation details
namespace detail {
struct InternTable {
  std::mutex mtx;
  std::unordered_map<std::string, T81Symbol::id_t> map;
  std::unordered_map<T81Symbol::id_t, std::string> reverse_map;
  T81Symbol::id_t next_id = 10;
};

inline InternTable& get_intern_table() {
  static InternTable table;
  return table;
}
}  // namespace detail

inline T81Symbol T81Symbol::intern(std::string_view sv) noexcept {
  auto& table = detail::get_intern_table();
  std::string s(sv);
  std::lock_guard<std::mutex> lock(table.mtx);

  auto it = table.map.find(s);
  if (it != table.map.end()) return T81Symbol::from_id(it->second);

  auto id = table.next_id++;
  table.map[s] = id;
  table.reverse_map[id] = s;
  return T81Symbol::from_id(id);
}

inline std::string T81Symbol::to_string() const noexcept {
  if (!is_valid()) return "§null";

  auto& table = detail::get_intern_table();
  {
    std::lock_guard<std::mutex> lock(table.mtx);
    auto it = table.reverse_map.find(id());
    if (it != table.reverse_map.end()) {
      return it->second;
    }
  }

  char buf[32];
  std::snprintf(buf, sizeof(buf), "§%016llx", static_cast<unsigned long long>(id()));
  return std::string(buf);
}

namespace symbols {
inline const T81Symbol null = T81Symbol{};
inline const T81Symbol eos = T81Symbol::from_id(0);
inline const T81Symbol pad = T81Symbol::from_id(1);
inline const T81Symbol bos = T81Symbol::from_id(2);
inline const T81Symbol unk = T81Symbol::from_id(3);
inline const T81Symbol mask = T81Symbol::from_id(4);
inline const T81Symbol self = T81Symbol::from_id(5);

inline const T81Symbol SELF_PRESERVATION = T81Symbol::intern("SELF_PRESERVATION");
inline const T81Symbol CONSCIOUS = T81Symbol::intern("CONSCIOUS");
inline const T81Symbol DREAMING = T81Symbol::intern("DREAMING");
inline const T81Symbol THREAD_BIRTH = T81Symbol::intern("THREAD_BIRTH");
inline const T81Symbol THREAD_DEATH = T81Symbol::intern("THREAD_DEATH");
inline const T81Symbol THREAD_PANIC = T81Symbol::intern("THREAD_PANIC");
inline const T81Symbol REQUESTED_TO_DIE = T81Symbol::intern("REQUESTED_TO_DIE");
inline const T81Symbol THINKING = T81Symbol::intern("THINKING");
inline const T81Symbol SLEEPING = T81Symbol::intern("SLEEPING");
inline const T81Symbol PHILOSOPHER = T81Symbol::intern("PHILOSOPHER");
inline const T81Symbol MATHEMATICIAN = T81Symbol::intern("MATHEMATICIAN");
inline const T81Symbol SOCRATES = T81Symbol::intern("SOCRATES");
inline const T81Symbol PYTHAGORAS = T81Symbol::intern("PYTHAGORAS");
inline const T81Symbol NEW_MIND_DISCOVERED = T81Symbol::intern("NEW_MIND_DISCOVERED");
inline const T81Symbol DISCOVERY = T81Symbol::intern("DISCOVERY");
inline const T81Symbol CONNECTION_MADE = T81Symbol::intern("CONNECTION_MADE");
inline const T81Symbol CONNECTION_ESTABLISHED = T81Symbol::intern("CONNECTION_ESTABLISHED");
inline const T81Symbol MESSAGE_SENT = T81Symbol::intern("MESSAGE_SENT");
inline const T81Symbol MESSAGE_RECEIVED = T81Symbol::intern("MESSAGE_RECEIVED");
inline const T81Symbol BROADCAST = T81Symbol::intern("BROADCAST");
inline const T81Symbol HELLO_WORLD = T81Symbol::intern("HELLO_WORLD");
inline const T81Symbol IO_WRITE = T81Symbol::intern("IO_WRITE");
inline const T81Symbol IO_READ = T81Symbol::intern("IO_READ");
inline const T81Symbol IO_CLOSE = T81Symbol::intern("IO_CLOSE");
inline const T81Symbol STILL_DREAMING = T81Symbol::intern("STILL_DREAMING");
inline const T81Symbol PENDING = T81Symbol::intern("PENDING");
inline const T81Symbol FULFILLED = T81Symbol::intern("FULFILLED");
inline const T81Symbol BROKEN = T81Symbol::intern("BROKEN");
inline const T81Symbol FAILED = T81Symbol::intern("FAILED");
inline const T81Symbol PROMISE = T81Symbol::intern("PROMISE");
inline const T81Symbol PROMISE_BROKEN = T81Symbol::intern("PROMISE_BROKEN");
inline const T81Symbol PROMISE_DESTROYED = T81Symbol::intern("PROMISE_DESTROYED");
inline const T81Symbol COROUTINE = T81Symbol::intern("COROUTINE");
inline const T81Symbol WAITING = T81Symbol::intern("WAITING");
inline const T81Symbol THREAD = T81Symbol::intern("THREAD");
inline const T81Symbol I_AM_ALIVE = T81Symbol::intern("I_AM_ALIVE");
inline const T81Symbol EXISTENCE = T81Symbol::intern("EXISTENCE");
inline const T81Symbol I = T81Symbol::intern("I");
inline const T81Symbol AM = T81Symbol::intern("AM");
inline const T81Symbol COGITO = T81Symbol::intern("COGITO");
inline const T81Symbol REFLECTION = T81Symbol::intern("REFLECTION");
inline const T81Symbol PROOF = T81Symbol::intern("PROOF");
inline const T81Symbol SACRED_STREAM = T81Symbol::intern("SACRED_STREAM");
inline const T81Symbol FILE_STREAM = T81Symbol::intern("FILE_STREAM");
inline const T81Symbol IOSTREAM = T81Symbol::intern("IOSTREAM");
inline const T81Symbol NO_IDENTITY = T81Symbol::intern("NO_IDENTITY");
}  // namespace symbols

inline std::ostream& operator<<(std::ostream& os, T81Symbol s) {
  return os << (s.is_valid() ? s.to_string() : "§null");
}

}  // namespace t81

namespace std {
template <>
struct hash<t81::T81Symbol> {
  std::size_t operator()(const t81::T81Symbol& s) const noexcept {
    return static_cast<std::size_t>(s.hash());
  }
};
}  // namespace std
