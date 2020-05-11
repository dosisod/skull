# src/common/wegex.h

#### `bool wegex_wc_cmp(wchar_t control, wchar_t wc)`
Returns true if `control` is comparable to `wc`.

If `control` is `'\n'`, then return wether `wc` is a number or not.
If `control` is `'\b'`, then return wether `wc` is a hexadecimal nuber (byte) or not.

Else, return wether `control` and `wc` are equal.

#### `bool wegex_match(const wchar_t *wegex, const wchar_t *match)`
Returns true if the `wegex` string matches the `match` string.

"wegex" is a wide-char regex implementation, and uses different syntax from regex:
* Modifiers go BEFORE the character, for example, `abc*` would be `ab*c` in wegex.
* "wegex" strings must match exactly, as in, they are like doing `^x$` in regex.

