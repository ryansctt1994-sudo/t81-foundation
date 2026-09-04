# T81 Foundation — Deterministic Ternary Architecture

T81 Foundation is an experimental computing stack for balanced-ternary data types, a ternary-oriented instruction-set architecture, deterministic execution research, language/runtime tooling, and policy-governed execution.

The project targets **bit-exact behavior on explicitly defined and tested surfaces**. It does not claim whole-system determinism across every platform, high-stakes certification, cryptographic assurance, or operational authority merely because a component is normative or version-frozen.

See [EVIDENCE_AND_AUTHORITY.md](EVIDENCE_AND_AUTHORITY.md).

## Evidence boundary

```text
normative specification != operational authority
version frozen != externally verified
local repro gate != independent reproduction
policy enforcement != safety guarantee
verified surface != whole-stack guarantee
```

A determinism claim must name the exact operation/surface, implementation and commit, input domain, platform/toolchain set, and reproduction result. JIT, accelerator, external-library, concurrency, I/O, and model-execution behavior require their own evidence.

This writable repository exists in the `ryansctt1994-sudo` namespace while related repositories also exist under `t81dev`. Namespace similarity is not provenance proof; cite the exact repository URL and commit SHA for every result.

## Core architecture

T81 is organized into four conceptual layers:

1. **Foundation** — TISC ISA and ternary data-type contracts.
2. **Execution** — interpreter/runtime surfaces and experimental Trace-JIT work.
3. **Governance** — Axion-style policy checks and resource/authority constraints.
4. **Application** — T81Lang and higher-level consumers.

A specification can be the normative source of truth for what an implementation is supposed to do. “Normative” and “frozen” are change-control terms, not claims of infallibility, certification, or real-world authority.

## Component status

| Component | Project maturity | Claim boundary |
|---|---|---|
| TISC ISA | version-frozen v1 contract | normative project specification; changes require version control |
| Ternary data types | stable/frozen core contracts | deterministic claims limited to explicitly tested surfaces |
| T81VM | beta | runtime under continued verification |
| Axion policy engine | alpha | configured policy-enforcement behavior only; not a general safety guarantee |
| T81Lang | beta implementation / draft normative language spec | implementation and language-spec maturity remain separate |
| Trace-JIT | experimental | no inherited determinism/performance guarantee |
| Hanoi Kernel | archived concept | design history only |

## Quick start

Prerequisites:

- CMake 3.16+
- a compatible C++20/23 compiler
- Python 3 for repository verification scripts

```bash
git clone https://github.com/ryansctt1994-sudo/t81-foundation.git
cd t81-foundation
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
python3 scripts/ci/t81lang_repro_gate.py --t81-bin build/t81 --check
```

A successful run is local evidence for the exact commit/environment exercised. It is not independent reproduction and must not be generalized to untested targets.

Example source:

```t81
fn main() {
    print("Hello, Deterministic World!");
    let a: trit = 1;
    let b: trit = -1;
    print(a + b);
}
```

Subject to the current CLI contract:

```bash
./build/t81 compile hello.t81 -o hello.tisc
./build/t81 run hello.tisc
```

## Documentation precedence

The repository contains different document roles. Use these labels rather than “authority” levels:

| Resource | Role |
|---|---|
| `spec/` | normative project specifications |
| `docs/architecture/OVERVIEW.md` | architecture and system boundaries |
| `docs/status/PROJECT_CONTROL_CENTER.md` | status/tracking surface |
| `docs/governance/` | governance/change-control policies |
| `book/book-en/README.md` | explanatory monograph |
| `docs/T81_FOUNDATION_PROJECT_PROFILE.md` | public technical profile |

If documents conflict, the appropriate normative spec controls implementation intent for its scoped contract, but evidence of conformance still comes from tests/reproduction rather than document rank.

## Governance layer boundary

Axion-style controls are intended to mediate execution according to configured policies. Tests can establish that particular rules are enforced on particular execution paths. They cannot establish that a policy is ethically correct, complete, secure, clinically safe, legally compliant, or sufficient for a high-stakes deployment.

## Determinism and reproducibility

The repository's reproducibility gates are valuable engineering controls. Report their outputs with:

- exact repository + commit SHA;
- clean/dirty state;
- command and configuration;
- platform, CPU/GPU where relevant, OS and compiler/runtime versions;
- raw output/receipt;
- scope and unsupported/skipped surfaces.

Independent reproduction additionally requires an organizationally independent witness and artifact-bound returned evidence.

## Security and high-stakes use

T81 is research/engineering software unless a specific release or component has separate qualification evidence. Do not infer suitability for cryptography, medical systems, critical infrastructure, financial controls, autonomous safety functions, or other high-stakes use from determinism, balanced-ternary representation, formal specification, or policy enforcement alone.

## Contributing

See `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md`, and the governance documentation. Changes to normative/frozen surfaces require the relevant version/change-control process and corresponding conformance evidence.

## License

MIT. See [LICENSE](LICENSE).

Copyright © 2024-2026 T81 Foundation.
