# Workflow Action Pinning Audit

## Summary

- Total `uses:` references: **47**
- Pinned to commit SHA: **8**
- Tag/major-version references: **38**
- Unclassified references: **1**

## Tag/Major References (Migration Candidates)

| Workflow | Line | Reference |
| --- | ---: | --- |
| `.github/workflows/bench.yml` | 63 | `actions/upload-artifact@v4` |
| `.github/workflows/ci.yml` | 26 | `actions/checkout@v6` |
| `.github/workflows/ci.yml` | 29 | `actions/setup-python@v6` |
| `.github/workflows/ci.yml` | 41 | `lycheeverse/lychee-action@v2` |
| `.github/workflows/ci.yml` | 113 | `actions/checkout@v6` |
| `.github/workflows/ci.yml` | 152 | `actions/cache@v4` |
| `.github/workflows/ci.yml` | 160 | `actions/cache@v4` |
| `.github/workflows/ci.yml` | 171 | `ilammy/msvc-dev-cmd@v1` |
| `.github/workflows/ci.yml` | 229 | `actions/upload-artifact@v4` |
| `.github/workflows/ci.yml` | 239 | `actions/upload-artifact@v4` |
| `.github/workflows/ci.yml` | 247 | `actions/upload-artifact@v4` |
| `.github/workflows/ci.yml` | 289 | `actions/checkout@v6` |
| `.github/workflows/ci.yml` | 330 | `actions/download-artifact@v4` |
| `.github/workflows/ci.yml` | 336 | `actions/download-artifact@v4` |
| `.github/workflows/ci.yml` | 375 | `actions/download-artifact@v4` |
| `.github/workflows/ci.yml` | 381 | `actions/download-artifact@v4` |
| `.github/workflows/ci.yml` | 419 | `actions/checkout@v6` |
| `.github/workflows/ci.yml` | 443 | `actions/upload-artifact@v4` |
| `.github/workflows/ci.yml` | 461 | `actions/checkout@v6` |
| `.github/workflows/ci.yml` | 487 | `actions/checkout@v6` |
| `.github/workflows/ci.yml` | 516 | `actions/checkout@v6` |
| `.github/workflows/ci.yml` | 544 | `actions/checkout@v6` |
| `.github/workflows/repro-ledger.yml` | 19 | `actions/checkout@v6` |
| `.github/workflows/repro-ledger.yml` | 85 | `actions/upload-artifact@v4` |
| `.github/workflows/runtime-contract.yml` | 21 | `actions/checkout@v6` |
| `.github/workflows/runtime-contract.yml` | 27 | `actions/checkout@v6` |
| `.github/workflows/sidebar.yml` | 23 | `actions/checkout@v6` |
| `.github/workflows/sidebar.yml` | 28 | `actions/setup-python@v6` |
| `.github/workflows/sidebar.yml` | 45 | `peter-evans/create-pull-request@v8` |
| `.github/workflows/static.yml` | 22 | `actions/checkout@v6` |
| `.github/workflows/static.yml` | 27 | `actions/setup-node@v6` |
| `.github/workflows/static.yml` | 48 | `peter-evans/create-pull-request@v8` |
| `.github/workflows/t81lang-repro-hash-refresh.yml` | 17 | `actions/checkout@v6` |
| `.github/workflows/t81lang-repro-hash-refresh.yml` | 45 | `peter-evans/create-pull-request@v8` |
| `.github/workflows/toc.yml` | 23 | `actions/checkout@v6` |
| `.github/workflows/toc.yml` | 28 | `actions/setup-python@v6` |
| `.github/workflows/pdf.yaml` | 17 | `actions/checkout@v6` |
| `.github/workflows/pdf.yaml` | 33 | `actions/upload-artifact@v4` |

## SHA-Pinned References

| Workflow | Line | Reference |
| --- | ---: | --- |
| `.github/workflows/codeql.yml` | 32 | `actions/checkout@34e114876b0b11c390a56381ad16ebd13914f8d5` |
| `.github/workflows/codeql.yml` | 35 | `github/codeql-action/init@d3ced5c96c16c4332e2a61eb6f3649d6f1b20bb8` |
| `.github/workflows/codeql.yml` | 40 | `github/codeql-action/autobuild@d3ced5c96c16c4332e2a61eb6f3649d6f1b20bb8` |
| `.github/workflows/codeql.yml` | 43 | `github/codeql-action/analyze@d3ced5c96c16c4332e2a61eb6f3649d6f1b20bb8` |
| `.github/workflows/release.yml` | 33 | `actions/checkout@34e114876b0b11c390a56381ad16ebd13914f8d5` |
| `.github/workflows/release.yml` | 51 | `anchore/sbom-action@fbfd9c6c189226748411491745178e0c2017392d` |
| `.github/workflows/release.yml` | 57 | `sigstore/cosign-installer@faadad0cce49287aee09b3a48701e75088a2c6ad` |
| `.github/workflows/release.yml` | 68 | `softprops/action-gh-release@5be0e66d93ac7ed76da52eca8bb058f665c3a5fe` |

## Unclassified References

| Workflow | Line | Reference |
| --- | ---: | --- |
| `.github/workflows/pdf.yaml` | 20 | `docker://marpteam/marp-cli:v3.6.0` |

## Recommendation

- Convert high-trust workflows (`ci.yml`, `release.yml`, `runtime-contract.yml`) to SHA-pinned actions first.
- Keep Dependabot enabled for GitHub Actions so pinned SHAs advance through reviewable PRs.

