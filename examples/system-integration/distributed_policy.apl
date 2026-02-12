(policy
  (max-instructions 2000)
  (allowed-segments [CODE, STACK, HEAP, TENSOR])
  (require-reflection-cycle 3)       // Limit refinement cycles
  (max-reflection-depth 1)           // Shallow reflection only
  (max-tensor-memory 512MB)          // Allow large tensors for distributed workload
  (require-commit-verification true) // Patches must be verified
  (allowed-tensor-hashes ["sha3:1234abcd..." "sha3:5678efgh..."]) // Whitelist shards
)
