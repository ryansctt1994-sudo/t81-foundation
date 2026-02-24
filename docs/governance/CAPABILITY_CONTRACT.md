# T81 Capability Contract & Governance

## Capability Contract

The T81 Foundation is committed to providing a stable, secure, and deterministic platform. This contract defines the guarantees we provide to users and developers.

### 1. Determinism
We guarantee bit-exact reproducibility for all execution traces across supported platforms, given the same inputs and configuration.

### 2. Stability
*   **TISC ISA**: Versioned and frozen. Breaking changes require a major version bump.
*   **Public APIs**: Semantically versioned. Deprecation warnings provided one cycle in advance.

### 3. Security
All execution is sandboxed. Capability-based access control is enforced at the kernel level.

## Governance Model

The project is governed by a meritocratic process.

### Roles
*   **Maintainers**: Review and merge PRs, steer technical direction.
*   **Contributors**: Submit PRs, report issues.
*   **Users**: Provide feedback.

### Decision Making
Technical decisions are made through the RFC (Request for Comments) process.
