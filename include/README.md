# include

Public header surface for T81.

## Layout
- `include/t81/`: canonical public API headers.
- Subfolders map to runtime/compiler domains (axion, vm, tisc, frontend, tensor, bigint, canonfs, etc.).

## Contract
- Headers under `include/t81/` define externally consumable interfaces.
- Prefer stable APIs and additive changes.
- Behavior changes must be backed by tests and, when relevant, spec/doc updates.

## Related
- Detailed API map: `include/t81/README.md`
- Architecture context: `ARCHITECTURE.md`
