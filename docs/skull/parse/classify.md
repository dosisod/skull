# skull/parse/classify

```c
static void classify_token(Token *const token)
```

> Classify the token `token`.

```c
void classify_tokens(Token *head)
```

> Classify all tokens pointed to from `token`.

```c
static bool is_type_str(const char32_t *const name)
```

> Returns true if `name` is a type string.

```c
static bool is_reserved_str(const char32_t *const str)
```

> Returns true if a `str` is a reserved.

```c
static bool is_constant_integer_str(const char32_t *str)
```

> Returns true if `str` is a valid hex/octal/binary/decimal representation
> of an integer.
> \
> Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1010`, `0o777`

```c
static bool is_constant_float_str(const char32_t *str)
```

> Returns true if `str` is a valid float (with decimal).
> \
> Examples: `123.0`, `-123.0`, `0.0`, `Infinity`

```c
static bool is_constant_bool_str(const char32_t *const str)
```

> Returns true if `str` is a valid bool (`true` or `false`).

```c
static bool is_constant_rune_str(const char32_t *const str)
```

> Returns true if `str` is a valid rune.
> \
> Examples: `'x'`, `'\n'`, `'\xFF'`, and `' '`.
> Won't work: `''`, `'\'`, `'x '`, or `' x'`.

```c
static bool is_constant_str_str(const char32_t *const str)
```

> Returns true if `str` is a valid string constant.
> \
> Examples: `""` and `"hello"`.

```c
bool is_valid_identifier_str(const char32_t *str)
```

> Returns true if `str` is a valid identifer.

