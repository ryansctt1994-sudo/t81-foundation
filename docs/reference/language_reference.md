# T81 Language Reference

This document provides a reference for the T81 programming language (T81Lang), including syntax, types, and cognitive tier constructs.

## Types

### Primitive Types

| Type | Description | Example Literal |
| :--- | :--- | :--- |
| `T81Int` | Base-81 Integer | `123t81` |
| `T81Float` | Base-81 Float | `1.23t81` |
| `T81Fraction`| Rational Number | `1/3` (computed) |
| `Bool` | Boolean | `true`, `false` |
| `Symbol` | Symbolic Atom (T243) | `:my_symbol` |
| `T81String` | String | `"hello"` |
| `T81Bytes` | Byte sequence | `T81Bytes("abc")` |

### Infinite Types (T19683)

| Type | Description | Example Literal |
| :--- | :--- | :--- |
| `InfiniteCanonicalForm` | Infinite Structure Seed | `∞{seed_expr}` |

## Statements & Blocks

### Cognitive Tier Blocks

T81Lang supports dedicated blocks for specific cognitive tiers:

#### Reflective (T729)

```t81
reflect {
    // Reflective logic
}
```

#### Recursive (T2187)

Recursive functions are defined using `recurse`:

```t81
recurse factorial(n) {
    base -> 1t81;
    step -> n * factorial(n - 1t81);
}
```

#### Distributed (T6561)

Distributed logic uses the `distributed` block:

```t81
distributed {
    // Distributed logic
}
```

#### Infinite (T19683)

Infinite-tier logic uses the `infinite` block:

```t81
infinite {
    // Infinite logic
}
```

## Keywords

`fn`, `let`, `var`, `const`, `if`, `else`, `while`, `for`, `loop`, `break`, `continue`, `return`, `match`, `record`, `enum`, `type`, `module`, `export`, `reflect`, `recurse`, `distributed`, `infinite`.
