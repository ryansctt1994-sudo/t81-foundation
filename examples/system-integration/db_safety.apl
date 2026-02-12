(policy
  (max-instructions 5000)
  (allowed-segments [CODE, STACK, HEAP]) // No TENSOR access needed for this example
  (max-stack 1024)                       // Limit stack depth for recursive tree operations
  (require-commit-verification true)     // Structural changes must be verified
  (max-reflection-depth 2)               // Allow deeper reflection for complex logic
  (require-reflection-cycle 5)           // Allow multiple evolutionary steps
)
