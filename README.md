# [![CI](https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml/badge.svg)](https://github.com/t81dev/t81-foundation/actions/workflows/ci.yml)

# T81 Foundation Manifesto & Portal

> We turn the AI reproducibility crisis into a deterministic ledger. T81 is no longer a theoretical ternary playground; it is the production-ready stack that proves every model, tensor, and trace can be audited, replayed, and trusted.

## The Reproducibility Crisis Tally
- Benchmarks diverge whenever environments or numerics drift; probabilistic floating-point paths refuse third-party verification.
- Axion traps evade silent overflow, but most platforms never surface the logs that matter.
- Tensor blobs, weights dumps, and deployment scripts are scattered across unverified storage, so claims about accuracy or robustness cannot be reproduced.
- The missing artifact is not compute; it is a deterministic stack that enforces canonical encoding, Axion-safe overflow, and reproducible tracing from weights to inference.

## The T81 Determinism Manifesto
- **Canonical numerics**: Balanced ternary arithmetic (`T81Int`, `T81Fraction`, `T81Float`, `T81Tensor`) guarantees `encode(decode(x)) ≡ x` and Axion traps on any overflow/underflow. No hidden carries; no silent corruption.
- **Deterministic execution**: `t81 compile` → `t81 run` emits HanoiVM bytecode with Axion metadata so every run is replayable, observable, and audit-ready.
- **Tensor transparency**: `t81 weights import/quantize/info` tracks SHA3-512 metadata, trit statistics, and CanonFS hints, ensuring tensors carry provenance all the way into HanoiVM.
- **Evidence pipeline**: Benchmark runs (`t81 benchmark`), Axion traces (`./scripts/capture-axion-trace.sh`), and canonical artifacts (`docs/benchmarks.md`) become the reproducibility ledger researchers, auditors, and regulators can cite.
- **Specification-first governance**: All semantics live in `/spec/`; any change touching determinism, encoding, or logging must follow the RFC path. The stack enforces zero hidden nondeterminism.

## Axion Guarantee
```cpp
t81::axion::Context axion;
auto value = t81::T81Int<81>::kMaxValue;
auto result = value + t81::T81Int<81>(1); // Axion traps overflow before corruption
```

## Portal: Join the Stack
1. **Build the manifest**  
   ```bash
   git clone https://github.com/t81dev/t81-foundation.git
   cd t81-foundation
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --parallel
   ctest --test-dir build --output-on-failure
   ```  
   This standard pipeline is your baseline truth; every contributor and researcher runs it before generating comparisons.

2. **Extend & verify the ledger**  
   - Use `ctest --test-dir build -R "fuzz|property|axion" --schedule-random` to stress Axion controls.
   - Reference [`spec/index.md`](spec/index.md), [`spec/t81lang-spec.md`](spec/t81lang-spec.md), [`spec/tisc-spec.md`](spec/tisc-spec.md), [`spec/t81vm-spec.md`](spec/t81vm-spec.md), and [`spec/t81-data-types.md`](spec/t81-data-types.md) for normative behavior.
   - Coordinate changes with tests under `/tests/`; every new public face needs proof, every semantics change needs an RFC.

3. **Publish deterministic artifacts**  
- Regenerate [`docs/benchmarks.md`](docs/benchmarks.md) via the `t81 benchmark` shortcut (which builds `build/benchmarks/benchmark_runner`) whenever numerics, compilers, or tensor tooling shift.
   - Capture Axion trace logs (`axion_policy_runner.log`, `vm_bounds_trace.log`, etc.) with `./scripts/capture-axion-trace.sh` and archive them alongside each benchmark run.
   - Use [`docs/assets/`](docs/assets/) and [`docs/guides/weights-integration.md`](docs/guides/weights-integration.md) to explain how weights land in deterministic HanoiVM deployments.

4. **Stay informed & aligned**  
   - Keep an eye on [`docs/system-status.md`](docs/system-status.md) for ongoing risks and investigative priorities.
   - Cite [`CLAUDE.md`](CLAUDE.md) and [`ANALYSIS.md`](ANALYSIS.md) for contextual guidance when documenting limitations or anomalies.
   - Propose RFCs under [`spec/rfcs/`](spec/rfcs/) before touching the constitution; new APIs go under [`include/t81/`](include/t81/) and are paired with tests in [`tests/`](tests/).

## Repository Portal Map

| Compass | Purpose |
| --- | --- |
| [`spec/`](spec/) | Constitutional governance; normative semantics only change through RFCs. |
| [`include/t81/`](include/t81/) | Public `t81::v1` headers; add APIs here with Axion-safe guarantees. |
| [`src/`](src/) | Implementation of compiler, HanoiVM, tensor tooling, and benchmarks. |
| [`tests/`](tests/) | Automated proofs of determinism; every API change gets coverage. |
| [`benchmarks/`](benchmarks/) | Benchmark generators and runners that power `docs/benchmarks.md`. |
| [`docs/`](docs/) | Manifest insights, benchmark reports, onboarding guides, and system status. |
| [`examples/`](examples/) | Canonical T81Lang demos and deployment snippets. |
| [`legacy/hanoivm/`](legacy/hanoivm/) | Frozen historical reference; read-only. |

## Next Steps for Researchers
- Read the canonical specs ([`spec/index.md`](spec/index.md), [`spec/t81lang-spec.md`](spec/t81lang-spec.md), [`spec/tisc-spec.md`](spec/tisc-spec.md), [`spec/t81vm-spec.md`](spec/t81vm-spec.md), [`spec/t81-data-types.md`](spec/t81-data-types.md)).
- Explore [`docs/onboarding.md`](docs/onboarding.md) and [`docs/cpp-quickstart.md`](docs/cpp-quickstart.md) before making contributions.
- Run deterministic weight pipelines (`t81 weights import`, `t81 weights quantize`, `weights.load(...)`) and update [`docs/benchmarks.md`](docs/benchmarks.md) with each meaningful change.
- Archive Axion traces (`build/artifacts/*axion*.log`) with every benchmark/experiment so future reviewers can replay the exact behavior.
