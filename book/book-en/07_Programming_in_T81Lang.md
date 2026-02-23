# Chapter 7: Programming in T81Lang

**Status: Implemented**

T81Lang is the primary high-level language for the T81 Virtual Machine. It is designed to be safe, deterministic, and amenable to formal verification. While the T81VM can execute any valid TISC bytecode, T81Lang provides a structured, type-safe way to author these programs.

The language syntax is inspired by Rust and Swift, emphasizing immutability, strong typing, and expression-oriented control flow.

## 7.1 Design Philosophy

*   **Deterministic by Default**: No undefined behavior. Floating-point math uses software-defined deterministic implementations (`dmath`).
*   **Expression Oriented**: Most constructs (like `match` and blocks) return values.
*   **Explicit State**: Mutation is opt-in via `var`.
*   **No Hidden allocations**: Memory usage is predictable and often stack-allocated or strictly managed by the Axion policy.
*   **Tier-Aware**: Advanced features (recursion, reflection) require explicit tier promotion or are gated by policy.

## 7.2 Syntax Basics

### 7.2.1 Comments
Comments start with `//` and extend to the end of the line. Multiline comments use `/* ... */`.

```rust
// This is a comment
let x = 10; // Inline comment
/*
  This is a multiline comment.
*/
```

### 7.2.2 Variables
Variables are declared with `let` (immutable) or `var` (mutable). Type inference is supported, but explicit types are recommended for clarity in public APIs.

```rust
let immutable_val: i32 = 42;
// immutable_val = 43; // Error: Cannot assign to immutable variable

var mutable_val = 10;
mutable_val = 20; // OK
```

### 7.2.3 Blocks and Scope
Scopes are defined by curly braces `{}`. Blocks are expressions and return the value of their last expression if the semicolon is omitted. This is similar to Rust.

```rust
let result = {
    let a = 5;
    let b = 10;
    a + b // Returns 15
};
```

## 7.3 Data Types

T81Lang supports a set of primitives that map directly to T81VM storage types.

### 7.3.1 Primitives

| Type | Description | Literal Example |
| :--- | :--- | :--- |
| `i32` | 32-bit signed integer | `42` |
| `i64` | 64-bit signed integer | `100` |
| `T81Float` | Deterministic floating point | `3.14t81` |
| `T81BigInt` | Arbitrary precision integer | `123456789t81` |
| `bool` | Boolean (true/false) | `true` |
| `char` | Unicode Scalar Value | `'a'` |

**Note**: `T81Float` and `T81BigInt` literals require the `t81` suffix to distinguish them from standard host types or generic numbers during parsing. `3.14` is a host double (only allowed in constant expressions), while `3.14t81` is a runtime `T81Float`.

### 7.3.2 Collections

*   **T81String**: A UTF-8 string. Immutable.
    ```rust
    let greeting: T81String = "Hello";
    ```
*   **T81Vector[T, N]**: A fixed-size array of type `T` with size `N`. Stack-allocated.
    ```rust
    let vec: T81Vector[i32, 4] = [1, 2, 3, 4];
    let val = vec[0];
    ```
*   **Slice[T]**: A view into a vector or array.

### 7.3.3 Enums (Option and Result)
T81Lang includes standard `Option` and `Result` types for error handling and nullable values. These are algebraic data types.

```rust
let maybe: Option[i32] = Some(10);
let missing: Option[i32] = None;

let success: Result[i32, T81String] = Ok(200);
let failure: Result[i32, T81String] = Err("Something went wrong");
```

### 7.3.4 Bitwise Operations
T81Lang supports standard bitwise operators for integer types (`i32`, `i64`, `T81BigInt`, `T81Uint`, etc.).

*   `&` (AND)
*   `|` (OR)
*   `^` (XOR)
*   `~` (NOT)
*   `<<` (Left Shift)
*   `>>` (Arithmetic Right Shift)
*   `>>>` (Logical Right Shift)

## 7.4 Control Flow

### 7.4.1 Conditionals
`if` expressions return values.

```rust
let max = if (a > b) { a } else { b };
```

### 7.4.2 Pattern Matching
The `match` expression is the primary way to handle Enums and conditional logic. It must be exhaustive.

```rust
let val = match (maybe) {
    Some(v) => v * 2,
    None => 0,
};
```

### 7.4.3 Loops
T81Lang supports `while` and `for` loops. Infinite loops are generally discouraged and may be terminated by Axion policies (Gas limit).

**While Loop:**
```rust
var i = 0;
while (i < 10) {
    print(i);
    i = i + 1;
}
```

**For Loop (Range):**
```rust
for i in (0..10) {
    print(i);
}
```

## 7.5 Functions

Functions are declared with `fn`. They must explicitly state their return type (unless void). Functions are pure by default unless they interact with IO or global state (which requires capabilities).

```rust
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

fn main() -> i32 {
    let sum = add(5, 10);
    print(sum);
    return 0;
}
```

Every program must have a `main` function that acts as the entry point. It typically returns `i32` (status code) or `Result[i32, Error]`.

## 7.6 Structures and Methods

**Status: Beta**

T81Lang supports defining custom data structures.

```rust
struct Point {
    x: T81Float,
    y: T81Float,
}

impl Point {
    fn new(x: T81Float, y: T81Float) -> Point {
        return Point { x: x, y: y };
    }

    fn distance_to_origin(self) -> T81Float {
        // Uses dmath implicitly
        return (self.x * self.x + self.y * self.y).sqrt();
    }
}
```

## 7.7 Axion Integration

When compiling T81Lang, you can attach an Axion policy. The compiler ensures that the generated TISC bytecode adheres to basic safety rules, but runtime policies (like instruction limits) are enforced by the VM.

```bash
t81 compile source.t81 --policy strict.policy -o program.tisc
```

You can also use policy-aware intrinsics:

```rust
if (Axion.check_tier(3)) {
    // Safe to use Tier 3 features
    recurse_deeply();
} else {
    // Fallback
    iterative_approach();
}
```

## 7.8 Examples

### 7.8.1 Hello World

```rust
// examples/hello_world.t81
fn main() -> i32 {
    print("Hello, World!");
    return 0;
}
```

### 7.8.2 Vector Addition

```rust
// examples/vector_add.t81
fn add_vectors(a: T81Vector[i32, 4], b: T81Vector[i32, 4]) -> T81Vector[i32, 4] {
    // In future versions, vector ops will be intrinsic.
    // For now, we manually construct the result.
    return [
        a[0] + b[0],
        a[1] + b[1],
        a[2] + b[2],
        a[3] + b[3]
    ];
}

fn main() -> i32 {
    let v1: T81Vector[i32, 4] = [1, 2, 3, 4];
    let v2: T81Vector[i32, 4] = [10, 20, 30, 40];

    let result = add_vectors(v1, v2);

    // Validate
    if (result[0] == 11) {
        print("Test Passed");
    }
    return 0;
}
```
