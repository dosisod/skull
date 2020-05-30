# src/common/wegex.h

#### `bool wegex_wc_cmp(const wchar_t *begin, const wchar_t *end, wchar_t wc)`
Returns true if `wc` matches a wegex group or a single character.

If char at `begin` is `'\n'`, then return whether `wc` is a number or not.
If char at `begin` is `'\b'`, then return whether `wc` is a hexadecimal number (byte) or not.
If char at `begin` is `'\a'`, then return whether `wc` is an ASCII alpha character: `A-Z`, `a-z`
If char at `begin` is `'\f'`, then return whether `wc` is an alphanumeric character: `0-9`, `A-Z`, `a-z`, or locale specific alphanumeric character.

If char at `begin` is `'['`, then return wether `wc` matches any character within `begin` and `end`.

Else, return wether `wc` and the char at `begin` are equal.

#### `const wchar_t *find_next_wegex(const wchar_t* wegex)`
Returns a pointer to the next searchable wegex group.

If `wegex` is pointing to a `'['` character, return the corresponding `']'`.
Else, return the passed wegex.

#### `bool wegex_match(const wchar_t *wegex, const wchar_t *match)`
Returns true if the `wegex` string matches the `match` string.

"wegex" is a wide-char regex implementation, and uses different syntax from regex:
* Modifiers go BEFORE the character, for example, `abc*` would be `ab*c` in wegex.
* "wegex" strings must match exactly, as in, they are like doing `^x$` in regex.
* Character groups can be used, for example, `+[123]+[abc]` matches `"321cba"`.

