# src/parse/classify.h

#### `bool is_type_str(const wchar_t *name)`
Returns true if `token` is a type string.

#### `bool is_keyword_str(const wchar_t *str)`
Returns true if a `str` is a keyword.

#### `bool is_constant_integer_str(const wchar_t *str)`
Returns true if string is a valid hex/octal/binary/decimal integer.

Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1010`, `0o777`

#### `bool is_constant_float_str(const wchar_t *str)`
Returns true if string is a valid float (with decimal).

Examples: `123.0`, `-123.0`, `0.0`

#### `bool is_constant_bool_str(const wchar_t *str)`
Returns true if the string is a valid bool (`true` or `false`).

#### `bool is_constant_char_str(const wchar_t *str)`
Returns true if the string is a valid char.

Examples: `'x'` and `' '`.
Won't work: `''`, `'x '`, or `' x'`.

#### `bool is_constant_str_str(const wchar_t *str)`
Returns true if the string is a valid string constant.

Examples: `""` and `"hello"`.

#### `bool is_valid_identifier_str(const wchar_t *str)`
Returns true the passed character the start of a valid identifier.

#### `void classify_token(token_t *token)`
Classify the token `token`.

#### `void classify_tokens(token_t *head)`
Starting at token `token`, go through and classify each token in linked list.

