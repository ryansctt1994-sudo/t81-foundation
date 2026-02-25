# Implementation vs. Specification Matrix

| Subsystem | Specification Status | Implementation Status | Spec-Code Alignment | Drift Risk | Notes |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Data Types** | `spec/t81-data-types.md` (Frozen) | **Implemented** | High | Low | Core types (`Trit`, `Tryte`) are fully supported in `core/types`. |
| **TISC ISA** | `spec/tisc-spec.md` (Frozen) | **Implemented** | High | Low | Full opcode coverage in `core/isa` and `core/vm`. |
| **T81VM** | `spec/t81vm-spec.md` (Beta) | **Partial** | Medium | Medium | Dispatch loop is solid; IO and memory model are still evolving. |
| **T81Lang** | `spec/t81lang-spec.md` (Draft) | **Stubbed** | Low | High | `lang/stdlib` is largely empty (`std/` exists, but compiler core is missing). |
| **Axion Kernel** | `spec/axion-kernel.md` (Draft) | **Partial** | Medium | Medium | Basic policy engine exists (`kernel/axion`), but kernel features are incomplete. |
| **Cognitive Tiers** | `spec/cognitive-tiers.md` (Draft) | **Stubbed** | None | High | `experimental/tiers` contains only documentation placeholders. |

## Recommendation Actions

1.  **Spec-Lock**: Freeze `T81VM` spec for v1.0.
2.  **Implementation Focus**: Prioritize `T81Lang` compiler implementation to match the spec.
3.  **Cleanup**: Remove or properly mark stubbed implementations in `experimental/tiers` to avoid confusion.
4.  **CI Enforcement**: Add "Spec Coverage" metrics to CI pipeline.
