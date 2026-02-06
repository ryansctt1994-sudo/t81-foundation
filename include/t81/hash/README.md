# t81/hash — Canonical Base-81 & CanonHash

This directory provides the canonical implementations for hashing and Base-81 encoding.

## Files

- `base81.hpp`

  - `encode_base81(bytes) -> std::string`
  - `decode_base81(string) -> std::vector<uint8_t>`
  - Uses the canonical 81-character alphabet (0-9, A-Z, a-z, and 19 mathematical symbols).

- `canonhash.hpp`

  - `hash_bytes(data) -> CanonHash81`
  - `hash_string(s) -> CanonHash81`
  - Uses SHA3-512 truncated to 256 bits for the digest path.

## Design

1. **Base-81 Encoding:** Deterministic and invertible; no whitespace or padding.
2. **CanonHash81:** A 256-bit hash (32 bytes). Its string representation is a Base-81 encoded string of these bytes.
3. **Compatibility:** Legacy "b81:" prefixes are no longer accepted.

## Safety

The `CanonHash81` uses a cryptographically strong digest (SHA3), but the system's overall security depends on how these hashes are used and verified within the Axion kernel policies.
