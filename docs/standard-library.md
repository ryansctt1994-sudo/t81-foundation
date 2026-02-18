# T81Lang Standard Library

This document describes the standard library modules available in T81Lang.
These modules are located in `src/lang/std/`.

## Generic Call Syntax

User-defined generic functions support both inferred and explicit call-site type arguments:
- Inferred: `id(7)`
- Explicit: `id[i32](7)`
- Partial explicit with inference fallback: `first[i32](7, "tail")`

Explicit type arguments must match the function generic arity and argument types deterministically.
Calls that leave return-affecting generic parameters unresolved fail deterministically, for example:
- `fn none_of[T]() -> Option[T] { return None; }`
- `none_of()` -> `Cannot infer generic parameter 'T' for function 'none_of'.`

## Core Modules

### `std.core`
Basic definitions and utilities.
- `fn assert(condition: bool) -> void`: Aborts execution if condition is false (`std.core.assert` lowers to a deterministic trap path on false).
- `fn debug(msg: T81String) -> void`: Prints a debug message (`std.core.debug` lowers to builtin `print` and therefore accepts the same scalar-printable types).
- `fn unwrap_or(opt: Option[T], default: T) -> T`: Option helper (`std.core.unwrap_or` lowers via `OPTION_IS_SOME` / `OPTION_UNWRAP`).

### `std.math`
Mathematical constants and functions (backed by `include/t81/std/math.hpp`).
**Note:** Transcendental functions (`sin`, `cos`, `exp`, etc.) currently rely on host `double` precision and are platform-dependent.
**Current frontend/VM status:** `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sinh`, `cosh`, `tanh`, `sqrt`, `exp`, `log`, `pow`, and `clamp` are wired.
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
- `fn stream() -> T81String`: Deterministic stream token alias (currently lowers to stable symbolic token `std.io.stream`).
- `fn net() -> T81String`: Deterministic network token alias (currently lowers to stable symbolic token `std.io.net`).

## Data Structure Modules

### `std.collections`
- `fn len(v: Vector[T]) -> i32`: Deterministic vector length for frontend `Vector[T]` values.
- `fn is_empty(v: Vector[T]) -> bool`: Deterministic vector emptiness check.
- `fn first(v: Vector[T]) -> T`: Deterministic first-element lookup; empty vectors are rejected deterministically.
- `fn last(v: Vector[T]) -> T`: Deterministic last-element lookup; empty vectors are rejected deterministically.
- `fn push(v: Vector[T], value: T) -> Vector[T]`: Deterministic append returning a new vector value.
- `fn pop(v: Vector[T]) -> Vector[T]`: Deterministic drop-last returning a new vector value; empty vectors are rejected deterministically.
- `fn list() -> Vector[T81String]`: Deterministic empty list constructor (runtime-backed string-vector value).
- `fn map() -> Vector[T81String]`: Deterministic empty map-constructor placeholder value (runtime-backed string-vector form).
- `fn set() -> Vector[T81String]`: Deterministic empty set-constructor placeholder value (runtime-backed string-vector form).
- `fn tree() -> Vector[T81String]`: Deterministic empty tree-constructor placeholder value (runtime-backed string-vector form).
- `fn graph() -> Vector[T81String]`: Deterministic empty graph-constructor placeholder value (runtime-backed string-vector form).

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
- `fn from_bytes(b: T81Bytes) -> T81String`: Deterministic bytes-to-text alias.
- `split`, `join`: implemented with deterministic semantics (`split` preserves empty segments; `join` supports empty vectors) backed by `Vector[T81String]` VM runtime handles.

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
- `fn split(b: T81Bytes, sep: T81Bytes) -> Vector[T81Bytes]`: Deterministic split preserving empty segments. Empty separator is rejected.
- `fn join(parts: Vector[T81Bytes], sep: T81Bytes) -> T81Bytes`: Deterministic join over byte-vector parts.
- `fn to_string(b: T81Bytes) -> T81String`: Deterministic bytes-to-text conversion alias.
- `fn from_string(s: T81String) -> T81Bytes`: Deterministic text-to-bytes conversion alias.
- `fn T81Bytes(s: T81String) -> T81Bytes`: Deterministic conversion constructor.

### `std.symbol`
Deterministic symbol helpers.
- `fn intern(s: T81String) -> T81String`: Deterministic symbol interning alias (currently represented as interned string handles in T81Lang).
- `fn to_string(sym: T81String) -> T81String`: Deterministic symbol-to-text alias.
- `fn eq(a: T81String, b: T81String) -> bool`: Deterministic symbol equality alias.
- `fn ne(a: T81String, b: T81String) -> bool`: Deterministic symbol inequality alias.

## System & Runtime

### `std.sys`
- `fn exit(code: i32) -> void`: Deterministic terminate/trap alias (`std.sys.exit` lowers to `TRAP`).
- `fn time() -> T81Float`: Deterministic logical time alias (currently lowers to constant `0.0`).
- `fn entropy() -> i32`: Deterministic entropy-budget alias (currently lowers to constant `0`).
- `fn proof() -> T81String`: Deterministic proof token alias (currently lowers to stable symbolic token `std.sys.proof`).

### `std.async`
- `fn yield() -> void`: Deterministic cooperative-yield alias (currently a no-op).
- `fn sleep(duration: T81Float) -> void`: Deterministic sleep alias (currently a no-op after float-compatible type check).
- `fn thread() -> T81String`: Deterministic thread token alias (currently lowers to stable symbolic token `std.async.thread`).
- `fn promise() -> T81String`: Deterministic promise token alias (currently lowers to stable symbolic token `std.async.promise`).

### `std.tensor`
- `std.tensor.load("...") -> i32`: Load weights via deterministic literal-path alias (literal required by current frontend contract).
- `fn from_list(values: Vector[i32]) -> Tensor`: Build tensor from deterministic integer vector literal.
- `fn matmul(a: Tensor, b: Tensor) -> Tensor`: Matrix multiplication.
- `fn vec_add(a: Tensor, b: Tensor) -> Tensor`: Deterministic vector/tensor addition alias.

### `std.agent`
- `fn self_reflect() -> void`: Trigger deterministic reflection opcode (`META_REFLECT`).
