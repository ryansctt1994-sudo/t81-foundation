# `include/t81/crypto`

Public cryptographic hash interfaces.

## Key Headers
- `sha3.hpp`: SHA3-512 APIs.

## Exposed APIs
- `sha3_512(std::span<const uint8_t>) -> std::array<uint8_t, 64>`
- `sha3_512_hex(std::span<const uint8_t>) -> std::string`

## Notes
- Keep output canonical and deterministic.
- Any algorithm/format change must be versioned and test-covered.
