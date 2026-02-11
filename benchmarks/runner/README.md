# benchmarks/runner

Source files for the benchmark runner executable.

## Files
- `benchmark_runner.cpp`: main registration/entry
- `arith_throughput.cpp`, `limb_arith_throughput.cpp`: arithmetic throughput
- `negation_speed.cpp`, `overflow_detection.cpp`, `packing_density.cpp`: core ternary lanes
- `memory_bandwidth.cpp`, `roundtrip_accuracy.cpp`: memory/accuracy checks
- `t81lang_compile.cpp`: compile-path measurements

## Expectations
- Keep benchmark inputs deterministic.
- Avoid embedding environment-specific assumptions in result logic.
