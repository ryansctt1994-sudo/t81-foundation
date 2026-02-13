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
Mathematical constants and functions (deterministic, backed by `include/t81/std/math.hpp`).
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
- `fn to_string(v: any) -> T81String`: Convert to string (decimal for numbers).
- `fn split(s: T81String, delim: char) -> List[T81String]`: Split string.
- `fn join(parts: List[T81String], delim: T81String) -> T81String`: Join strings.

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
