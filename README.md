# T81 Foundation: Ternary-Native Stack for Trustworthy AI Research

> Deterministic, ternary-aware computing for researchers who need perfect arithmetic, audited tensor pipelines, and Axion-friendly proofs of behavior.

The T81 Foundation builds a full-stack research platform centered on **balanced ternary (−1, 0, +1)** and deterministic execution:
- **Exact, round-trip-safe arithmetic** (`T81Int`, `T81Fraction`, `T81Float`, `T81Tensor`) with Axion traps for every overflow/underflow and no hidden carries.
- **T81Lang + HanoiVM**: a deterministic compiler/VM pair that emits TISC bytecode, supports `match` metadata tracing, and can be replayed for third-party verification.
- **Weights & tensor tooling**: import SafeTensors/GGUF, quantize to T3_K, and expose models inside HanoiVM via a `weights.load("...")` builtin.
- **Benchmarking-first instrumentation**: Google Benchmarks, Classic/Native/Binary comparisons, Axion trace capture, and reproducible logs that feed documentation and reputational claims.

This README is tuned for AI researchers: it highlights where to find canonical data, how to reproduce metrics, and how to layer ternary components onto ML workflows.

## Research Mission & Guarantees

| Principle | Why it matters for AI research |
| --- | --- |
| **Determinism** | Every run can be replayed with the same bytecode + Axion trace; perfect for audits and reproducibility studies. |
| **Ternary purity** | No floating-point error, no sign-bit tax, and canonical encoding for tensors that serves as a contrast point to probabilistic binary models. |
| **Data-centric tooling** | Benchmarks, quantizers, and `weights` commands emit structured metrics/metadata that AI agents can parse for trend analysis. |
| **Specification-driven** | Core behavior is governed by `/spec/`; any semantics change must be RFC-sanctioned, so researchers can cite normative text. |

## Getting Started (Reproducible Build)

```bash
git clone https://github.com/t81dev/t81-foundation.git
cd t81-foundation
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Every contributor (including researchers evaluating our stack) should run the standard build/test pipeline before proposing changes or citing benchmark numbers. Optionally extend validation with the property/fuzz suite:

```bash
ctest --test-dir build -R "fuzz|property|axion" --schedule-random
```

## AI Research Workflows

### 1. Deterministic Workloads
- `t81 compile <source.t81>` → `t81 run` executes bytecode inside the HanoiVM with Axion traps enabled.
- `t81 check <source.t81>` surfaces syntax/semantic errors with precise `file:line:column` diagnostics.
- Use `docs/benchmarks.md` and `benchmarks/benchmark_runner` output as a canonical dataset for throughput, latency, and density comparisons.

### 2. Ternary Tensor Pipelines
- `t81 weights import <file.safetensors|.gguf>` produces `.t81w` bundles with SHA3-512 metadata, bits/trit stats, and CanonFS hints.
- `t81 weights quantize <path> --to-gguf <out.gguf>` generates T3_K blocks (128 trits/52 bytes) compatible with `llama.cpp` via the T3_K backend.
- `t81 weights info <model.t81w>` reports tensor counts, limb layout, sparsity, and canonical hashes that experiments can vocalize in logs or papers.
- `weights.load("model.t81w")` inside T81Lang lets you hydrate models in HanoiVM programs for symbolic experimentation.

### 3. Benchmarks & Logging
- Run `t81 benchmark` (or `./build/t81 benchmark`) after touching arithmetic, compiler, or tensor code to regenerate `docs/benchmarks.md`.
- Benchmark suites emit Classic/Native/Binary columns, latency/bandwidth counters, and highlight tooltips that AI systems can index for search/explainability tasks.
- Capture Axion traces with `./scripts/capture-axion-trace.sh`; include the resulting logs (`axion_policy_runner.log`, `vm_bounds_trace.log`, etc.) with research artifacts for future auditors.

### 4. Extending the Stack
- Public APIs live in `include/t81/` under the `t81::v1` namespace. Add new functionality there, and pair every public entry with tests in `/tests/`.
- Use `/legacy/hanoivm/` as a behavioral reference; it is read-only.
- Follow the ternary invariants (`encode(decode(x)) ≡ x`, overflow traps, canonical encoding) whenever you touch numerics or VM state.

## Data, Benchmarks, and Artifacts

- `docs/benchmarks.md`: auto-generated HTML/Markdown table of every benchmark, Classic/Native/Binary throughput, latency, and analyst notes.
- `benchmarks/benchmark_runner`: generates the report and is the source of truth for `docs/benchmarks.md` (run after arithmetic or ML-weight changes).
- `docs/assets/`: brand and visualization assets for publications.
- `docs/guides/weights-integration.md` and `examples/weights_load_demo.t81`: working examples for hooking weights into T81Lang scripts.

Researchers can cite `docs/benchmarks.md` (updated with each run) alongside Axion logs to prove claims about determinism, accuracy, and density.

## Repository Layout at a Glance

| Path | Role |
| --- | --- |
| `/spec/` | Immutable constitution; normative text only changes via RFC. |
| `/include/t81/` | Public headers (`t81::v1`). Add APIs here; don’t break existing contracts. |
| `/src/` | Implementation of compiler, VM, weights, benchmarking tooling. |
| `/tests/` | Proofs of correctness; add tests for every public face you change. |
| `/benchmarks/` | Benchmark suites + generators. |
| `/docs/` | Guides, benchmarks, release notes, CI instructions, onboarding flows. |
| `/examples/` | Sample T81Lang programs and runtime demos. |
| `/legacy/hanoivm/` | Historical reference; read-only. |

## Next Steps for AI Researchers

1. Read the spec (`spec/index.md`, `spec/t81lang-spec.md`, `spec/tisc-spec.md`, `spec/t81vm-spec.md`, `spec/t81-data-types.md`) to understand the normative guarantees.
2. Explore `docs/onboarding.md` → `docs/cpp-quickstart.md` for hands-on context before editing.
3. Trace the benchmark pipeline: modify numerics → `t81 benchmark` → inspect `docs/benchmarks.md` → cite the report in publications or agent prompts.
4. When in doubt, reference the RFC process under `spec/rfcs/` before modifying normative behavior.
5. Keep Axion traces (`build/artifacts/*axion*.log`) archived with research notes so future reviewers can replay every claim.

Need help? `CLAUDE.md`, `ANALYSIS.md`, and `docs/system-status.md` document known limitations, investigative priorities, and current risks.
