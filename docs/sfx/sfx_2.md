# SFX 2 - Type System

Skull is a strict, strong, and statically typed langauge.

To be more persice:
* Everything is const by default
* Skull has no concept of `undefined`, `NULL`, `None`, etc.
* Implicit casts are not allowed. The must be explicitly written by the programmer.

### Const by Default

In Skull, variables are const by default:

```
int32 x = 0
x = 1  # error, cannot assign to const
```

To make a non-const variable, use `mut`:

```
mut int32 x = 0
x = 1
```

### `undefined` / `NULL` / `None`

Skull has no concept of "noneness". You cannot have a variable that is "undefined".

Functions without a return type (`void`) cannot be assigned to a variable.

### No Implicit Casts

In other programming languages, implicit casts may unintentially introduce errors:

```cpp
// C++

int x = 123.456;
```

Here, `123.456` is implicitly converted from `float` to `int`.

In Skull, you must explicitly cast to `int`:

```
int32 x = int32[123.456]
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

```
# pseudo code
Array[int32] x

if [x.length == 0] [
  # true
]
```

## Built-in Types

Below is a table of built-in types, and their C/C++ equivalents:

| Skull | C/C++ | Description |
|:----- |:----- |:----------- |
| `bool` | `bool` | Stores a `true` or `false` value. |
| `int8` | `int8_t` | Signed 8 bit int |
| `int16` | `int16_t` | Signed 16 bit int |
| `int32` | `int32_t` | Signed 32 bit int |
| `int64` | `int64_t` | Signed 64 bit int |
| `float32` | `float` | 32 bit floating number |
| `float64` | `double` | 64 bit floating number |
| `float128` | `long double` | 128 bit floating number |
| `decimal` | N/A | Lossless base10 decimal `*` |
| `char` | `wchar_t` | UTF-16 code point `*` |
| `str` | `wchar_t*` | String of UTF-16 characters `*`|
| `T` | `template<typename T>` | Templated type `*` |
| `auto` | `auto` | Automatically deduced type `*` |

`*`: Subject to change
