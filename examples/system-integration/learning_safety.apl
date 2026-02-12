(policy
  (require-reflection-cycle 5)    // Max 5 refinement cycles allowed
  (max-reflection-depth 2)       // No nested reflection beyond depth 2
  (require-commit-verification true)) // Patches must pass a secondary hash check
