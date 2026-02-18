# T81Lang Standard Library

This document describes the standard library modules available in T81Lang.
These modules are located in `src/lang/std/`.

## Core Modules

### `std.core`
Basic definitions and utilities.
- `fn assert(condition: bool) -> void`: Aborts execution if condition is false.
- `fn debug(msg: T81String) -> void`: Prints a debug message.
- `fn unwrap_or(opt: Option[i32], default: i32) -> i32`: Option helper.

### `std.math`
Mathematical constants and functions (backed by `include/t81/std/math.hpp`).
**Note:** Transcendental functions (`sin`, `cos`, `exp`, etc.) currently rely on host `double` precision and are platform-dependent.
- `PI`, `E`: Constants (high-precision).
- `fn sin(x: T81Float) -> T81Float`: Sine (radians).
- `fn cos(x: T81Float) -> T81Float`: Cosine (radians).
- `fn tan(x: T81Float) -> T81Float`: Tangent (radians).
- `fn asin(x: T81Float) -> T81Float`: Arc Sine.
- `fn acos(x: T81Float) -> T81Float`: Arc Cosine.
- `fn atan(x: T81Float) -> T81Float`: Arc Tangent.
- `fn sinh(x: T81Float) -> T81Float`: Hyperbolic Sine.
- `fn cosh(x: T81Float) -> T81Float`: Hyperbolic Cosine.
- `fn tanh(x: T81Float) -> T81Float`: Hyperbolic Tangent.
- `fn exp(x: T81Float) -> T81Float`: Exponential ($e^x$).
- `fn log(x: T81Float) -> T81Float`: Natural Logarithm.
- `fn pow(b: T81Float, e: T81Float) -> T81Float`: Power function.
- `fn sqrt(x: T81Float) -> T81Float`: Square Root.
- `fn clamp(v: T81Float, min: T81Float, max: T81Float) -> T81Float`: Clamping.

### `std.io`
Input and Output.
- `fn println(msg: T81String) -> void`: Print string with newline (implied by print currently).
- `fn print_int(val: i32) -> void`: Print integer.
- `fn print_float(val: T81Float) -> void`: Print float.

## Data Structure Modules

### `std.collections`
- Placeholder for Vector/Map utilities.

### `std.text`
String manipulation and formatting (backed by `include/t81/std/string.hpp`).
- `fn str_len(s: T81String) -> i32`: Deterministic string length.
- `fn str_is_empty(s: T81String) -> bool`: Deterministic emptiness check.
- `fn concat(a: T81String, b: T81String) -> T81String`: Deterministic string concatenation.
- `fn starts_with(s: T81String, prefix: T81String) -> bool`: Deterministic prefix check.
- `fn ends_with(s: T81String, suffix: T81String) -> bool`: Deterministic suffix check.
- `fn contains(s: T81String, needle: T81String) -> bool`: Deterministic substring check.
- `fn index_of(s: T81String, needle: T81String) -> i32`: Deterministic first-match index (`-1` if not found).
- `fn replace(s: T81String, needle: T81String, replacement: T81String) -> T81String`: Deterministic global replace (non-overlapping, left-to-right). Empty `needle` leaves `s` unchanged.
- `fn to_string(s: T81String|T81Bytes) -> T81String`: Deterministic conversion to text.
- `fn from_bytes(b: T81Bytes) -> T81String`: Module wrapper around `std.text.to_string`.
- `split`, `join`: planned, not yet exposed as T81Lang builtins.

### `std.bytes`
Deterministic bytes helpers.
- `fn len(b: T81Bytes) -> i32`: Deterministic byte-length.
- `fn is_empty(b: T81Bytes) -> bool`: Deterministic emptiness check.
- `fn concat(a: T81Bytes, b: T81Bytes) -> T81Bytes`: Deterministic concatenation.
- `fn starts_with(b: T81Bytes, prefix: T81Bytes) -> bool`: Deterministic prefix check.
- `fn ends_with(b: T81Bytes, suffix: T81Bytes) -> bool`: Deterministic suffix check.
- `fn contains(b: T81Bytes, needle: T81Bytes) -> bool`: Deterministic subsequence check.
- `fn index_of(b: T81Bytes, needle: T81Bytes) -> i32`: Deterministic first-match index (`-1` if not found).
- `fn replace(b: T81Bytes, needle: T81Bytes, replacement: T81Bytes) -> T81Bytes`: Deterministic global replace.
- `fn to_string(b: T81Bytes) -> T81String`: Deterministic bytes-to-text conversion alias.
- `fn T81Bytes(s: T81String) -> T81Bytes`: Deterministic conversion constructor.

## System & Runtime

### `std.sys`
- `fn exit(code: i32) -> void`: Terminate execution.
- `fn time() -> T81Float`: Logical time.

### `std.async`
- `fn yield() -> void`: Cooperative yield.
- `fn sleep(duration: T81Float) -> void`: Sleep.

### `std.tensor`
- `fn tensor_load(path: T81String) -> Tensor`: Load weights.
- `fn matmul(a: Tensor, b: Tensor) -> Tensor`: Matrix multiplication.

### `std.agent`
- `fn self_reflect() -> void`: Trigger reflection block.
