# System Status

## Component Health

| Component | Status | Spec Compliance | Test Coverage |
| :--- | :--- | :--- | :--- |
| **TISC ISA** | Stable | Derived from spec_map.yaml | Computed in CI |
| **T81VM** | Beta | Derived from spec_map.yaml | Computed in CI |
| **Axion Kernel** | Alpha | Derived from spec_map.yaml | Computed in CI |
| **T81Lang** | Experimental | Derived from spec_map.yaml | Computed in CI |
| **Hanoi VM** | Concept | Derived from spec_map.yaml | Computed in CI |

## Known Critical Issues

*   **Performance**: VM dispatch overhead is currently 2x target.
*   **Docs**: Significant drift between spec and implementation in Axion.
*   **Tools**: Debugger is currently broken on Windows.

## Maintenance Mode

*   **Legacy Parser**: Deprecated, security patches only.
