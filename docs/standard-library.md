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
Mathematical constants and functions.
- `PI`, `E`: Constants.
- `fn sin_rad(x: T81Float) -> T81Float`: Sine (radians).
- `fn cos_rad(x: T81Float) -> T81Float`: Cosine (radians).
- `fn tan_rad(x: T81Float) -> T81Float`: Tangent (radians).
- `fn to_degrees(rad: T81Float) -> T81Float`: Conversion.
- `fn to_radians(deg: T81Float) -> T81Float`: Conversion.
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
- Placeholder for String manipulation.

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
