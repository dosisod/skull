# src/parse/classify.c

#### `void classify_token(token_t *token)`
Classify the token `token`.

#### `void classify_tokens(token_t *head)`
Classify all tokens pointed to from `token`.

#### `bool is_type_str(const char32_t *name)`
Returns true if `name` is a type string.

#### `bool is_keyword_str(const char32_t *str)`
Returns true if a `str` is a keyword.

#### `bool is_constant_integer_str(const char32_t *str)`
Returns true if `str` is a valid hex/octal/binary/decimal representation of an integer.

Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1010`, `0o777`

#### `bool is_constant_float_str(const char32_t *str)`
Returns true if `str` is a valid float (with decimal).

Examples: `123.0`, `-123.0`, `0.0`, `Infinity`

#### `bool is_constant_bool_str(const char32_t *str)`
Returns true if `str` is a valid bool (`true` or `false`).

#### `bool is_constant_char_str(const char32_t *str)`
Returns true if `str` is a valid char.

Examples: `'x'` and `' '`.
Won't work: `''`, `'x '`, or `' x'`.

#### `bool is_constant_str_str(const char32_t *str)`
Returns true if `str` is a valid string constant.

Examples: `""` and `"hello"`.

#### `bool is_valid_identifier_str(const char32_t *str)`
Returns true if `str` is a valid identifer.

