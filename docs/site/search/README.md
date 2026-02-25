# Documentation Search Index

The `docs/site/search/` directory contains the search index and related helpers generated when you build the docs site (`cmake --build build --target docs`). Treat this README as the entry point before touching the generated files.

## Key files
- `index.html` – the front-end that presents the search box. Link to it from `docs/navigation.md` when describing how to discover content.  
- `search.js` – wiring for the client-side search experience; it loads the generated JSON tokens to serve instant results.  
- `search-index.json` – machine-generated tokens; do not edit manually because `cmake --build build --target docs` rewrites it every time the docs change.

## Workflow reminders
- Regenerate the search index after editing the content that appears on the docs site (`docs/`, `spec/`, etc.) by rerunning `cmake --build build --target docs`.  
- Use this README when someone asks for search improvements so the team knows not to hand-edit the generated artifacts. Instead, update the source markdown and rebuild.
