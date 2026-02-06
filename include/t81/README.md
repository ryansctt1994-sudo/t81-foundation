Here’s **`include/t81/README.md`**:

````md
# t81 C++ Headers

Modular, header-only C++ API for T81 data types and utilities.

## Modules

- `config.hpp` — feature toggles, version macros, portability attrs.
- `ternary.hpp` — `Trit` (−1/0/+1), `uint81_t` carrier.
- `bigint.hpp` — signed base-81 big integer (`T81BigInt`) with `add/sub/mul/mod/gcd`.
- `fraction.hpp` — signed rationals (`T81Fraction`), always reduced; denom > 0.
- `tensor.hpp` — row-major tensor (`T729Tensor`) with basic ops.
- `tensor/ops.hpp` — aggregator for extra tensor ops:
  - `tensor/transpose.hpp` — `ops::transpose(m)`
  - `tensor/slice.hpp` — `ops::slice2d(m, r0, r1, c0, c1)`
  - `tensor/reshape.hpp` — `ops::reshape(m, new_shape)` (one `-1` inference)
- `canonfs.hpp` — `CanonHash81`, `CanonRef`, hashing seam.
- `canonfs_io.hpp` — fixed 99-byte wire encode/decode helpers.
- `ir/opcodes.hpp` — minimal opcode enum.
- `ir/insn.hpp` — POD instruction format.
- `ir/encoding.hpp` — 32-byte binary encoding for `Insn`.

## Layout

- `axion/` — Axion context, policy, verdict, and engine helpers. Pair these headers with the policy language (see `policy/` and `spec/axion-kernel.md`) when instrumenting deterministic guards.
- `bigint/`, `tensor/`, `vm/`, `canonfs/`, `cli/`, `lang/`, `frontend/`, and `ir/` each expose targeted headers that compose into the `t81` binary or external consumers such as HanoiVM/out-of-tree runtimes.
- `weights.hpp` and `tensor.hpp` publish the canonical tensor types (`T729Tensor`) plus serialization hooks for the `.t81w` format referenced in [`docs/guides/weights-integration.md`](../docs/guides/weights-integration.md).
- `core/`, `support/`, and `detail/` are implementation scaffolding; keep them internal, avoid exposing their symbols, and prefer `t81.hpp` as the public entry point.
- `ternary.hpp`, `fraction.hpp`, `canonfs.hpp`, `conversion.hpp`, and the `tensor/ops.hpp` helpers showcase the round-trip safety required by the constitution (encode/decode invariants and Axion traps on ±∞).

## Usage

```cpp
#include <t81/t81.hpp>
#include <t81/tensor/ops.hpp>  // if you need extra tensor ops

using namespace t81;
T81BigInt a = T81BigInt::from_ascii("1.42.7"); // base-81 digits (MSB-first, '.'-separated)
T81BigInt b = T81BigInt::from_base81_string("1.80.5");
auto s = T81BigInt::add(a,b);

T729Tensor m({2,3}); m.data() = {1,2,3,4,5,6};
auto mt = t81::ops::transpose(m);
````

## Guidelines
- Prefer `#include <t81/t81.hpp>` when using the API from `src/` or `tests/`; the umbrella header inlines the rest of the canonical modules.
- Add new functionality only under a dedicated namespace (`t81::v1::...`) and keep implementations header-only unless there is a compelling performance reason. Public headers must never break ABI without a spec/RFC and corresponding tests in [`tests/`](../tests/).
- Follow the build/test ritual (`cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`, `cmake --build build --parallel`, `ctest --test-dir build --output-on-failure`) after touching these headers so Axion invariants remain verified.
- Document non-trivial additions in [`docs/`](../docs/), especially when you introduce new tensors, guards, or CLI helpers. Use [`docs/navigation.md`](../docs/navigation.md) and the relevant README (e.g., [`docs/guides/README.md`](../docs/guides/README.md), [`docs/spec/README.md`](../docs/spec/README.md)) to decide where to place that narrative before publishing.

## Notes

- `from_ascii(...)` expects canonical base-243 digits (`[+-]?d(.d)*` where `0<=d<243`), MSB-first. `from_base81_string(...)` handles base-81 digit strings (`0..80`).
- IO helpers are minimal and non-cryptographic; validate per CanonFS spec in production.

```
```
