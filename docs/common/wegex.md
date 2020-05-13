# src/common/wegex.h

#### `bool wegex_wc_cmp(const wchar_t *start, const wchar_t *end, wchar_t wc)`
Returns true if `wc` matches a wegex group or a single character.

If char at `start` is `'\n'`, then return wether `wc` is a number or not.
If char at `start` is `'\b'`, then return wether `wc` is a hexadecimal number (byte) or not.

If char at `start` is `'['`, then return wether `wc` matches any character within `start` and `end`.

Else, return wether `wc` and the char at `start` are equal.

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

