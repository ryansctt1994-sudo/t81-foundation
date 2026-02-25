#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include "t81/tracing/canonhash.hpp"

namespace t81 {

// Use the unified CanonHash81 from t81::hash.
using CanonHash81 = t81::hash::CanonHash81;

// Simple capability-style reference to a canonical object.
struct CanonRef {
  CanonHash81 target{};  // canonical hash (256-bit)
  uint16_t permissions{0};
  uint64_t expires_at{0};  // epoch seconds; 0 = never

  // Helpers
  static CanonRef make(const CanonHash81& t, uint16_t perms = 0, uint64_t exp = 0) {
    CanonRef r;
    r.target = t;
    r.permissions = perms;
    r.expires_at = exp;
    return r;
  }
};

// Optional permission bits
enum : uint16_t {
  CANON_PERM_READ = 1u << 0,
  CANON_PERM_WRITE = 1u << 1,
  CANON_PERM_APPEND = 1u << 2,
  CANON_PERM_ADMIN = 1u << 15
};

}  // namespace t81
