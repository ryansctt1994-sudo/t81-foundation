# `include/t81/axion`

Public Axion interfaces for policy/runtime integration.

## Key Headers
- `api.hpp`: primary Axion API surface.
- `verdict.hpp`: verdict model used by runtime policy checks.

## Status
- Supports current stub + incremental hardening workflow.
- Designed to allow backend evolution without destabilizing callers.

## Compatibility
- Keep API shape stable for VM/CLI integrations.
- Any semantic change to verdict handling should include test coverage.
