(policy
  (max-instructions 1000)      // Prevent infinite loops
  (allowed-segments [CODE, STACK, HEAP]) // Deny TENSOR/META access
  (log-level deterministic-trace))
