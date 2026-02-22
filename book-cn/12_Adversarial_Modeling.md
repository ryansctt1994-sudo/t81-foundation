# Chapter 12: Adversarial Modeling

## 12.1 Threat Model

**Status: Active**

T81 assumes an adversarial environment where the host hardware, operating system, and network peers may be malicious or faulty.

### 12.1.1 The "Libm Gap" Vector
A subtle attack vector exists where a malicious node exploits differences in the host's standard math library (`libm`).
*   **Attack**: Node A (x86) computes `sin(x)` slightly differently than Node B (ARM).
*   **Consequence**: State divergence leads to a consensus fork.
*   **Mitigation**: T81 enforces the use of `dmath` for all critical transcendental functions, ensuring bit-exact results regardless of the underlying `libm` implementation.

> **Verification**: `tests/cpp/test_property_float.cpp` verifies cross-platform consistency of `T81Float` operations.

### 12.1.2 Time-Travel Attacks
In a distributed system, a malicious peer might inject messages with future timestamps to manipulate the logical clock.
*   **Mitigation**: The `TickSync` protocol enforces monotonic clock updates. A message with a timestamp far in the future can be rejected or capped by policy.
*   **Verification**: `tests/cpp/tier4_vm_test.cpp` tests clock synchronization logic.

## 12.2 Side-Channel Resilience

**Status: Aspirational / Partial**

While T81 guarantees logical determinism, it does not currently guarantee constant-time execution for all operations. Timing side-channels may exist in the current implementation of `BigInt` multiplication and `dmath` functions.
