# Active Development Tasks

**Last Updated:** February 17, 2026

This document tracks immediate, actionable tasks for the T81 project.

## 1. Current Sprint: Cognitive Tiers & Performance

### Cognitive Tiers (T243–T19683)
- [ ] **Symbolic (Tier 1):** Implement `SymbolicGraph::rewrite` and `is_confluent` logic (currently stubs).
- [ ] **Reflective (Tier 2):** Connect `ReflectiveFrame` to Axion trace events for full introspection.
- [ ] **Recursive (Tier 3):** Implement `Recursor` evaluation loop and depth proof verification.
- [ ] **Distributed (Tier 4):** Implement `NodeState` synchronization and gossip protocol logic.
- [ ] **Infinite (Tier 5):** Implement `InfiniteCanonicalForm` lazy expansion logic.

### Core Runtime
- [ ] **JIT:** Advance `runtime/jit` from experimental research to prototype backend.
- [ ] **CanonFS:** Optimize `PersistentDriver` for high-throughput tensor I/O.
- [ ] **Float:** Complete "Partial Polyfill" remediation for full float division determinism (software implementation).

### Tooling
- [ ] **CLI:** Add `t81 trace export` to export Axion logs to JSON/CSV.
- [ ] **Debugger:** Enhance `t81 debug` with cognitive tier state inspection.

______________________________________________________________________

## 2. Recently Completed (Feb 2026)

The following tasks have been verified as complete:

1.  **Implement If-Expressions:** [Done]
2.  **Implement Block Expressions:** [Done]
3.  **Enhance Tensor/Vector Indexing:** [Done]
4.  **Fix Generic Parameter Validation:** [Done]
5.  **Implement Enum Namespaces and Scope Injection:** [Done]
6.  **Fix Lexer Dot Consumption:** [Done]
7.  **Implement Assignments to Array Indices:** [Done]
8.  **Resolve Type Aliases in Semantic Analysis:** [Done]
9.  **Implement `T81Fixed` and `T81Complex` Support:** [Done]
10. **Implement `T81Qutrit` and `T81Uint` Support:** [Done]
11. **Relax Vector Literal Typing:** [Done]
12. **Improve Error Recovery and Diagnostics:** [Done]
13. **Implement `std.text.split` / `std.text.join`:** [Done]
14. **Add String-Vector Runtime Support for T81Lang:** [Done]
15. **Add End-to-End split/join Test Matrix:** [Done]
16. **Expose `std.symbol` Frontend Aliases:** [Done]
17. **Add `std.tensor` CLI Fixture Goldens:** [Done]
18. **Add `std.tensor.matmul` CLI Fixture Coverage:** [Done]
19. **Expose `std.core.debug` Frontend Alias:** [Done]
20. **Expose `std.core.unwrap_or` Frontend Alias:** [Done]
21. **Expose `std.core.assert` Frontend Alias:** [Done]
22. **Expose `std.sys` / `std.async` / `std.agent` Frontend Aliases:** [Done]
23. **Align `std.sys` / `std.async` / `std.agent` Wrapper Modules:** [Done]
24. **Harden Unsupported `std.math` Alias Diagnostics:** [Done]
25. **Align `std.math` Wrapper Surface with Docs:** [Done]
26. **Implement First `std.math` Scalar Runtime Batch:** [Done]
27. **Implement Remaining `std.math` Transcendental Aliases:** [Done]
28. **Implement `std.math.clamp` Alias Lowering:** [Done]
29. **Implement `std.collections` Vector Length MVP:** [Done]
30. **Implement `std.collections` Element/Mutation Aliases:** [Done]
31. **Support Explicit Generic Call Type Arguments:** [Done]
32. **Support Partial Explicit Generic Call Arguments:** [Done]
33. **Harden Unresolved Generic Inference Diagnostics:** [Done]
34. **Expose `std.sys.entropy` Frontend Alias:** [Done]
35. **Expose `std.sys.proof` / `std.io.stream` / `std.io.net` Aliases:** [Done]
36. **Expose `std.async.thread` / `std.async.promise` Aliases:** [Done]
37. **Expose `std.collections` Roadmap Entry Aliases:** [Done]
38. **Upgrade `std.collections.list` to Real Vector Semantics:** [Done]
39. **Upgrade `std.collections.map` to Real Vector Semantics:** [Done]
40. **Upgrade `std.collections.set` to Real Vector Semantics:** [Done]
41. **Upgrade `std.collections.tree` / `std.collections.graph` to Real Vector Semantics:** [Done]
42. **Add `std.sys` / `std.io` / `std.async` Runtime Fixture Goldens:** [Done]
43. **Harden `std.collections` Map-Helper Diagnostics:** [Done]
44. **Implement `std.collections.map_size` / `std.collections.map_has`:** [Done]
45. **Implement Remaining `std.collections` Map Helpers:** [Done]
46. **Expose `std.sys.reflect` Alias:** [Done]
47. **Implement `std.collections.set_size` / `std.collections.set_has`:** [Done]
48. **Implement `std.collections.set_add` / `std.collections.set_remove`:** [Done]
49. **Materialize Typed Runtime Handles for `std.sys.proof`...:** [Done]
50. **Implement `std.collections.graph_edge_count`...:** [Done]
51. **Implement `std.collections.graph_remove_edge`:** [Done]
52. **Implement `std.collections.graph_neighbors`:** [Done]
