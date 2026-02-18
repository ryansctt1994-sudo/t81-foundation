# `tests/fixtures/t81lang_std_collections`

Fixture pack for CLI end-to-end coverage of `std.collections` aliases.

## Purpose
- Validate deterministic observable behavior for `std.collections.len`,
  `std.collections.is_empty`, `std.collections.first`, `std.collections.last`,
  `std.collections.push`, and `std.collections.pop`.
- Validate staged flat-map helpers `std.collections.map_size` and
  `std.collections.map_has` / `std.collections.map_put` /
  `std.collections.map_get` / `std.collections.map_remove` /
  `std.collections.map_keys` over `Vector[T81String]` map data encoded as
  alternating key/value entries.
- Validate staged set helpers `std.collections.set_size` and
  `std.collections.set_has` over `Vector[T81String]` data.
- Validate staged set mutation helpers `std.collections.set_add` and
  `std.collections.set_remove`, including deterministic duplicate-elision behavior.
- Validate staged graph helpers `std.collections.graph_edge_count`,
  `std.collections.graph_has_edge`, `std.collections.graph_add_edge`,
  `std.collections.graph_remove_edge`, and `std.collections.graph_neighbors`
  over flat edge-vector encodings (`[from0, to0, ...]`, odd tails ignored).
- Validate behavior for both frontend `Vector[i32]` values and
  `Vector[T81String]` values produced via `std.text.split`.
- Validate end-to-end generic call inference over nested container signatures
  (for example `Vector[T]` and `Option[T]`) through fixture execution and
  golden outputs.
