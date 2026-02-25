# Documentation Assets

This folder holds the static assets used by the generated documentation site (`docs/api`, `docs/site/search`, etc.).

## Contents
- `css/` – stylesheets for the static site. Keep them synchronized with the theme in `docs/site/_layouts` (especially if you regenerate via `cmake --build build --target docs`).  
- `js/` – support scripts for search widgets, navigation helpers, and interactive front-end behavior. Avoid editing them unless you know the site-builder pipeline (check `docs/site/_layouts/default.html`).  
- `img/` – shared images, icons, and diagrams referenced by `docs/` markdown. When adding new assets, update `docs/navigation.md` or the relevant guide to document their purpose.

## Maintenance notes
- These assets are consumed by the Doxygen/Docsify pipeline invoked through the build. Rebuild the docs if you change anything here (`cmake --build build --target docs`).  
- Treat this README as the orientation point before touching CSS/JS/Images so you can update `docs/navigation.md` or relevant guides simultaneously.
