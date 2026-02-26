(policy
  (tier 1)
  (max-instructions 100000)
  (max-stack 2048)
  (allowed-tensor-hashes ["__MODEL_HASH__"])
  (require-axion-event (reason "llama repro fixture"))
)
