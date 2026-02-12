# policy

Axion safety policies and alignment definitions.

## Contents
- `guards.axion`: A Tier 1 policy enforcing pattern matching safety (requiring match guards on `Option::Some`) and stack segment monitoring.
- `tier4_alignment.axion`: A Tier 4 policy defining execution constraints (instruction count, recursion depth, stack size) and requiring formal alignment verification events for higher-order cognitive processes.

## Usage
These `.axion` files are consumed by the Axion policy engine to validate runtime execution traces against safety and alignment requirements. They serve as the "law" that the runtime must obey.
