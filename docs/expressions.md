# Expressions

An expressions is a piece of code that returns a value. For example, `1 + 2` is
an expression. Expressions have types, which is determined by the operator (for
example, `+`, `-`, etc), and the operators (expressions on the left/right
hand-side of the operator).

## Simple Expressions

Simple expressions are expressions that do not have any operators. Examples of
such expressions would be `1`, `false`, `f()`, or `x`.

Here are all of the different types of simple expressions:

| Name | Type | Example(s) |
|------|------|---------|
| Integer literal | `Int` | `1`, `-1` |
| Binary literal | `Int` | `0b1111` |
| Octal literal | `Int` | `0o777` |
| Hex literal | `Int` | `0xFF` |
| Boolean literal | `Bool` | `true`, `false` |
| Floating-point literal | `Float` | `3.14`, `NaN`, `Infinity` |
| Rune literals | `Rune` | `'x'`, `'\x41'` |
| String literal | `Str` | `"hello world"` |
| Identifier | Type of the identifier | `x` |
| Function call | Return type of the function | `f()` |

## Unary Operators

Unary operators are operators that take one operand, for example, `not true`.

| Operation | Example | Types Supported |
|-----------|---------|-----------------|
| Not | `not true` | `Bool` |
| Unary negation | `- 1` | `Int`, `Float` |
| Dereference | `*pointer` | All pointer types |
| Reference | `&value` | All types |

## Binary Operators

Here is a run-down of the basic math operations that Skull supports:

| Operation | Example | Types Supported |
|-----------|---------|-----------------|
| Addition | `1 + 2` | `Int`, `Float` |
| Subtraction | `1 - 2` | `Int`, `Float` |
| Multiplication | `2 * 2` | `Int`, `Float` |
| Division | `10 / 2` | `Int`, `Float` |
| Modulous | `9 mod 2` | `Int`, `Float` |
| Binary shift-left | `0b1100 << 2` | `Int` |
| Binary shift-right | `0b1100 >> 2` | `Int` |
| Power | `1 ^ 2` | `Int`, `Float` |
| Is | `1 is 2` | `Int`, `Float`, `Rune`, `Str` |
| Isnt | `1 isnt 2` | `Int`, `Float`, `Rune`, `Str` |
| Less than | `1 < 2` | `Int`, `Float` |
| Less than or equal to | `1 <= 2` | `Int`, `Float` |
| Greater than | `1 > 2` | `Int`, `Float` |
| Greater than or equal to | `1 >= 2` | `Int`, `Float` |
| Boolean and | `true and false` | `Bool` |
| Boolean xor | `true xor false` | `Bool` |
| Boolean or | `true or false` | `Bool` |

Currently, Skull does not allow for integers in `and`, `or`, and `xor`
operations.
