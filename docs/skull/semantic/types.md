# skull/semantic/types

```c
typedef const char * Type
```

> Types are simply named strings.

```c
bool eval_bool(const Token *const token)
```

> Return bool converted from `token`

```c
double eval_float(const Token *const token, bool *err)
```

> Returns a float (actually a double) parsed from `token`.

```c
int64_t eval_integer(const Token *const token, bool *err)
```

> Returns a 64 bit integer parsed from `token`.

```c
char32_t eval_rune(const Token *const token, bool *err)
```

> Return "rune" converted from `token`.

```c
char32_t *eval_str(const Token *const token)
```

> Return string converted from `token`, or `NULL` if an error occurred.

```c
Type __attribute__((pure)) find_type(const char *const name)
```

> Returns pointer to type with name `name`.

```c
static void strip_underscore_num(char *str, char c)
```

> Strip underscores from `str`.
> \
> This function modifies the contents at `str`.
> \
> If `c` is defined, stop iterating when `c` is found.

