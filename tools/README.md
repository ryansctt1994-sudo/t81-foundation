# tools

Developer/inspection tools that are not part of the main `t81` command path.

## Contents
- `ir_inspector.cpp`: IR introspection helper
- `tisc_base81.cpp`: TISC/base81 utility tool
- `axion_policy_validator.py`, `validator.py`: policy/validation helpers
- `vscode-t81/`: VS Code extension assets for syntax support

## Guidelines
- Keep tools deterministic and script-friendly.
- If a tool becomes core workflow, consider routing it through `t81` CLI surface.
