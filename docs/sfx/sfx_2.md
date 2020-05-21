# SFX 2 - Type System

Skull is a strict, strong, and statically typed langauge.

To be more persice:
* Everything is const by default
* Skull has no concept of `undefined`, `NULL`, `None`, etc.
* Implicit casts are not allowed. The must be explicitly written by the programmer.

### Const by Default

In Skull, variables are const by default:

```
int x = 0
x = 1  # error, cannot assign to const
```

To make a non-const variable, use `mut`:

```
mut int x = 0
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
int x = int[123.456]
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
Array[int] x

if [x.length == 0] [
  # true
]
```

## Built-in Types

Below is a table of built-in types, and their C/C++ equivalents:

| Skull | C/C++ | Description |
|:----- |:----- |:----------- |
| `bool` | `bool` | Can only be `true` or `false` |
| `int` | `int64_t` | Stores a signed 63 bit integer |
| `float` | `long double` | 128 bit floating number |
| `char` | `wchar_t` | UTF-16 code point `*` |
| `str` | `wchar_t*` | String of UTF-16 characters `*`|
| `T` | `template<typename T>` | Templated type `*` |
| `auto` | `auto` | Automatically deduced type `*` |

`*` Subject to change
