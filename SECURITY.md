# Security Policy

> **Source of Truth:** This document defines the **supported versions** and **vulnerability reporting process**.

**Last Updated:** February 10, 2026

## 1. Supported Versions

We support the **current MAJOR version** and the **immediate previous MAJOR version** for critical security fixes.

| Version | Supported | Notes |
| :--- | :--- | :--- |
| **v1.x** | ✅ Yes | **Active:** Critical fixes and feature updates. |
| **v0.x** | ❌ No | **End of Life:** No longer supported. |
| **< v1.0** | ❌ No | Deprecated. |

## 2. Reporting a Vulnerability

Please report vulnerabilities privately through GitHub Security Advisories.

If advisory reporting is unavailable, open a private maintainer contact with:
- affected component/path,
- impact summary,
- minimal reproduction,
- suggested remediation (if known).

Do **not** publish exploit details in public issues before a fix is available.

## 3. Scope

Security-relevant areas include:
- VM execution and memory safety boundaries,
- Axion policy enforcement surfaces,
- CanonFS persistence and trace integrity,
- CLI/runtime artifact handling,
- workflow/CI supply-chain integrity.

## 4. Response Goals

- Initial triage acknowledgment: best effort within 72 hours.
- Severity assessment and mitigation plan: as quickly as practical based on impact.
- Coordinated disclosure after fix availability.

## 5. Handling Expectations

- Provide deterministic reproduction steps where possible.
- Avoid including secrets in reports.
- If the issue affects reproducibility or contract integrity, include expected vs actual deterministic outputs.
