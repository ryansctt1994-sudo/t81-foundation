# Gap Analysis Report

This report details the discrepancies between the T81Lang specification, the legacy PDF claims, and the actual codebase implementation.

## 1. Spec vs. Implementation Mismatches

### 1.1. Cognitive Tier Statements (`recurse`, `distributed`, `infinite`)
- **Spec Requirement:** The language supports tier-aware constructs to declare intent for Axion policy enforcement (Sections 0.6, 6.3, 6.4).
- **Implementation Status:** The parser recognizes `recurse`, `distributed`, and `infinite` (block) keywords and produces corresponding AST nodes (`RecurseStmt`, `DistributedStmt`, `InfiniteStmt`). However, `IRGenerator` treats these as **transparent blocks**, visiting their children without emitting any specific TISC opcodes or metadata instructions.
- **Risk:** Medium. While the code executes, the specific semantic intents (e.g., enforcing distribution policies or infinite loop convergence checks) are lost at the IR level. Axion cannot enforce policies on these blocks if they look like normal code in TISC.
- **Proposed Resolution:**
    1. Update spec to clarify that these are currently "syntactic markers" for future Axion integration.
    2. Roadmap item: Emit `META_REFLECT` or new `TIER_ENTER`/`TIER_EXIT` opcodes in `IRGenerator`.

### 1.2. User-Defined Generics
- **Spec Requirement:** Section 2.1 defines generic type syntax (`Vector[T]`, etc.) and the grammar supports generic function declarations (`fn foo[T](...)`).
- **Implementation Status:** The parser consumes generic type parameters for functions (`Parser::function`), but `IRGenerator::visit(FunctionStmt)` **ignores them entirely**. There is no monomorphization or type-erasure logic. The function is generated as a single label. This implies that generic functions only work if the underlying TISC code is effectively untyped/polymorphic (which it is, for `T81Int`), but type safety for `T` is not enforced during lowering for user-defined generics.
- **Risk:** High. Users might expect `fn identity[T](x: T) -> T` to enforce type constraints, but it likely compiles to a generic `MOV` without checks, or fails if `T` requires specific operations not supported by `T81Int`.
- **Proposed Resolution:** Explicitly mark User-Defined Generics as **NOT IMPLEMENTED** / **Experimental** in the new spec. Only built-in generics (`Option`, `Result`, `Vector`) are supported.

### 1.3. Standard Library Implementation
- **Spec Requirement:** Section 0.1 mentions `sin`, `cos` rely on host behavior.
- **Implementation Status:** The "Standard Library" is not a library written in T81Lang. It is a set of **compiler intrinsics** hardcoded in `IRGenerator::visit(CallExpr)`. Calls to `std.math.sin` are intercepted and replaced with `FSIN` opcodes.
- **Risk:** Low (Functionally correct).
- **Proposed Resolution:** Document this behavior. It means `std` functions cannot be passed as first-class values (function pointers) unless the compiler explicitly supports creating handles for them (which `canonical_stdlib_call_name` suggests it might not, as it maps names to strings, but `CallExpr` handling is specific).

## 2. PDF vs. Reality (Legacy Claims)

### 2.1. "AI-Driven Optimization"
- **PDF Claim:** (Inferred) The language includes AI optimizers or self-modifying code features.
- **Implementation Status:** **NOT IMPLEMENTED**. `IRGenerator` is a deterministic, single-pass compiler. `std.agent.self_reflect` exists as an intrinsic emitting `META_REFLECT`, but no "AI" logic is embedded in the compiler itself.
- **Resolution:** Deprecate. Define "AI" features strictly through the `std.agent` intrinsics and Axion policies, not magic compiler steps.

### 2.2. "GPU Acceleration"
- **PDF Claim:** (Inferred) Automatic GPU offloading.
- **Implementation Status:** **NOT IMPLEMENTED**. TISC is a stack/register VM running on CPU (or simulated). `Tensor` operations exist (`TMATMUL`), which *could* be GPU-accelerated by the VM implementation, but the language itself has no explicit GPU constructs beyond `distributed` (which is a no-op).
- **Resolution:** Clarify that T81Lang targets TISC; hardware acceleration is a VM implementation detail, not a language feature.

## 3. Implementation vs. Spec (Spec Missing)

### 3.1. `T81Fixed` and `T81Complex` Literals
- **Implementation Status:** `IRGenerator` specifically handles `T81Fixed(val)` and `T81Complex(r, i)` constructor calls by emitting specialized conversion or `MAKE_COMPLEX` opcodes.
- **Spec Requirement:** Not explicitly detailed in the "Core Grammar" or "Literals" section of `t81lang-spec.md`.
- **Resolution:** Add these to the "Supported Types" or "Intrinsics" section of the new spec.
