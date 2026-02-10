# Security Policy

## Supported Branch

Security fixes are published for `main`.

## Reporting a Vulnerability

Please report vulnerabilities privately through GitHub Security Advisories.

If advisory reporting is unavailable, open a private maintainer contact with:
- affected component/path,
- impact summary,
- minimal reproduction,
- suggested remediation (if known).

Do **not** publish exploit details in public issues before a fix is available.

## Scope

Security-relevant areas include:
- VM execution and memory safety boundaries,
- Axion policy enforcement surfaces,
- CanonFS persistence and trace integrity,
- CLI/runtime artifact handling,
- workflow/CI supply-chain integrity.

## Response Goals

- Initial triage acknowledgment: best effort within 72 hours.
- Severity assessment and mitigation plan: as quickly as practical based on impact.
- Coordinated disclosure after fix availability.

## Handling Expectations

- Provide deterministic reproduction steps where possible.
- Avoid including secrets in reports.
- If the issue affects reproducibility or contract integrity, include expected vs actual deterministic outputs.
