# T81 Foundation Navigation

Use this map before opening individual files—the Constitution, guides, tests, and artifacts all have dedicated homes, and this document highlights the living entry points and the archived material you can safely ignore or revisit later.

## Primary user pathways
- **Getting started**: [`docs/onboarding.md`](onboarding.md), [`docs/ai-quickstart.md`](ai-quickstart.md), and this [`docs/user-manual.md`](user-manual.md) outline prerequisites, build/test rituals, and the agent playbook. For CLI-centric onboarding, open [`docs/guides/cli-user-manual.md`](guides/cli-user-manual.md) and [`docs/guides/cli-toolkit.md`](guides/cli-toolkit.md).
- **Specification layers**: ask Axion for the canonical semantics in [`spec/index.md`](../spec/index.md). The layered order is [`spec/t81-data-types.md`](../spec/t81-data-types.md), [`spec/tisc-spec.md`](../spec/tisc-spec.md), [`spec/t81vm-spec.md`](../spec/t81vm-spec.md), [`spec/t81lang-spec.md`](../spec/t81lang-spec.md), [`spec/axion-kernel.md`](../spec/axion-kernel.md), and [`spec/cognitive-tiers.md`](../spec/cognitive-tiers.md). If you need the constitution-level text, refer to [`spec/constitution.md`](../spec/constitution.md).
- **Architecture & APIs**: [`ARCHITECTURE.md`](../ARCHITECTURE.md), [`include/t81/`](../include/t81/), [`src/`](../src/), and [`docs/guides/public-api-overview.md`](guides/public-api-overview.md) are your go-to references. After code changes, regenerate docs with `cmake --build build --target docs` and inspect `docs/api/html/index.html`.

## Domain-focused indexes
- **Guides**: see [`docs/guides/README.md`](guides/README.md) for a categorized TOC (CLI, Axion, semantics, weights/release). If a document has moved or the content is obsolete, you will usually find the archived copy with a `.archived` suffix in `docs/` (e.g., [`docs/developer-guide.md.archived`](developer-guide.md.archived)). Consider copying the updated TOC entry here when a guide is reborn under a new name.
- **Axion guides**: the [`docs/guides/axion/README.md`](guides/axion/README.md) entry point tells you which file to open based on whether you are decoding logs, authoring policies, or capturing runtime observability traces.
- **Documentation subtrees**:
  - `docs/benchmarks.md` summarizes the latest benchmark artifacts to keep in sync with `benchmarks/benchmark_runner`.
  - `docs/release.md`, `docs/hardware-roadmap.md`, and `docs/system-status.md` connect governance with observability; treat them as living documents.
  - `docs/spec/` mirrors the main spec. Update it only when you must produce a new topic that deserves web-friendly formatting (the Markdown here should reflect the canonical `spec/` text).
- **Artifacts & tests**:
- Axion logs live in [`build/artifacts/`](../build/artifacts/) . Keep them with deterministic artifacts and mention them in any research note (`docs/notes.md`).
- [`tests/`](../tests/) contains regression proof suites. If you add a public API, append tests and consider property testing (RapidCheck) per [`AGENTS.md`](../AGENTS.md).

## Cleanup cues
- Files ending with `.archived` are stale references; you can delete or update them only after confirming no living document references remain. Search for them before removing lines from [`docs/guides/README.md`](guides/README.md) or `docs/navigation.md`.
- Currently archived files that are safe to skip unless you need their history: [`docs/developer-guide.md.archived`](developer-guide.md.archived), [`docs/handover.md.archived`](handover.md.archived), and [`docs/tensor-guide.md.archived`](tensor-guide.md.archived). If any archive regains relevance, restore it under the living name and mention the change in both this map and [`docs/guides/README.md`](guides/README.md).
- When combining or retiring guides, create a redirect entry here and update `docs/guides/README.md` so readers immediately understand why the old file is still present.

## Need help discovering content?
- Run `rg --files docs` or open [`docs/search/index.html`](search/index.html) after building the docs site.
- For heavy context, read [`docs/user-manual.md`](user-manual.md) first, then follow the `docs/navigation.md` path into the appropriate subdirectory, keeping the AGENTS ritual (build/test) in mind.
