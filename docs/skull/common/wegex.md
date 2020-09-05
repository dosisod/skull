# skull/common/wegex.c

```c
bool wegex_match(const char *wegex, const char32_t *match)
```

> Returns true if the `wegex` string matches the `match` string.
> \
> "wegex" is a wide-char regex implementation, and uses different syntax from regex:
> * Modifiers go BEFORE the character, for example, `abc*` would be `ab*c` in wegex.
> * "wegex" strings must match exactly, as in, they are like doing `^x$` in regex.
> * Character groups can be used, for example, `+[123]+[abc]` matches `"321cba"`.

```c
const char __attribute__((pure)) *find_next_wegex(const char *wegex)
```

> Returns a pointer to the next searchable wegex group.
> \
> If `wegex` is pointing to a `'['` character, return the corresponding `']'`.
> Else, return the passed wegex.

```c
bool wegex_cmp(const char *begin, const char *end, char32_t c)
```

> Returns true if `c` matches a wegex group or a single character.
> \
> If char at `begin` is `'\n'`, then return whether `c` is a number or not.
> If char at `begin` is `'\b'`, then return whether `c` is a hexadecimal number (byte) or not.
> If char at `begin` is `'\a'`, then return whether `c` is an ASCII alpha character: `A-Z`, `a-z`
> If char at `begin` is `'\f'`, then return whether `c` is an alphanumeric character: `0-9`, `A-Z`, `a-z`
> \
> If char at `begin` is `'['`, then return wether `c` matches any character within `begin` and `end`.
> \
> Else, return wether `c` and the char at `begin` are equal.

