# SFX 2 - Type System

Skull is a strict, strong, and statically typed langauge.

To be more persice:
* Everything is const by default
* Skull has no concept of `undefined`, `NULL`, `None`, etc.
* Implicit casts are not allowed. The must be explicitly written by the programmer.

### Const by Default

In Skull, variables are const by default:

```python
x: int = 0

# or this
x := 0

x = 1  # error, cannot assign to const
```

To make a non-const variable, use `mut`:

```python
mut x: int = 0

# or this
mut x := 0

x = 1
```

### `undefined` / `NULL` / `None`

Skull has no concept of "noneness". You cannot have a variable that is "undefined".

Functions without a return type (`void`) cannot be assigned to a variable.

### No Implicit Casts/Conversions

In other programming languages, implicit conversions may unintentially introduce errors:

```cpp
// C++

int x = 123.456;
```

Here, `123.456` is implicitly converted from `float` to `int`.

In Skull, you must explicitly cast to `int`:

```python
x: int = int(123.456)
```

In most languages, there exists a concept of "truthy" and "falsy":

```javascript
// javascript

let arr = []
if (arr) {
  // true in this case
}
```

The value of `arr` is an object, and therefor is "truthy". In Skull, you must explicitly cast to `bool`:

```python
# pseudo code

x: Array(int)

if bool(x.length) {
  # true
}

elif x.length == 0 {
  # also true
}
```

## Built-in Types

Below is a table of built-in types, and their C/C++ equivalents:

| Skull | C/C++ | Description |
|:----- |:----- |:----------- |
| `bool` | `bool` | `true` or `false` |
| `int` | `int64_t` | 64 bit integer |
| `float` | `double` | 64 bit floating number |
| `rune` | `char32_t` | Single UTF-32 code point |
| `str` | `char32_t*` | String of UTF-32 code points |
| `T` | `template<typename T>` | Templated type `*` |
| `:=` | `auto` | Automatically deduce type |

`*` Subject to change
