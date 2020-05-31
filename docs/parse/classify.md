# src/parse/classify.h

#### `bool is_type_token(const token_t *token)`
Returns true if `token` is a type token.

#### `bool is_operator_token(const token_t *token)`
Returns true if `token` is an operator token.

#### `bool is_function_token(const token_t *token)`
Returns true if a token is a function token.

#### `bool is_constant_integer(const wchar_t *str)`
Returns true if string is a valid hex/binary/decimal integer.

Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1010`

#### `bool is_constant_integer_token(const token_t *token)`
Returns true if the passed token is an integer constant.

See above function for examples of valid inputs.

#### `bool is_constant_float(const wchar_t *str)`
Returns true if string is a valid float (with decimal).

Examples: `123.0`, `-123.0`, `0.0`

#### `bool is_constant_float_token(const token_t *token)`
Returns true if the passed token is a float constant.

See above function for examples of valid inputs.

#### `bool is_constant_bool(const wchar_t *str)`
Returns true if the string is a valid bool (`true` or `false`).

#### `bool is_constant_bool_token(const token_t *token)`
Returns true if the passed token is a boolean constant.

#### `bool is_constant_char(const wchar_t *str)`
Returns true if the string is a valid char.

Examples: `'x'` and `' '`.
Won't work: `''`, `'x '`, or `' x'`.

#### `bool is_constant_char_token(const token_t *token)`
Returns true if the passed token is a char constant.

Examples of valid inputs can be seen in the above function.

#### `bool is_constant_str(const wchar_t *str)`
Returns true if the string is a valid string constant.

Examples: `""` and `"hello"`.

#### `bool is_constant_str_token(const token_t *token)`
Returns true if the passed token is a string constant.

Examples of valid inputs can be seen in the above function.

#### `bool is_valid_identifier(const wchar_t *str)`
Returns true the passed character the start of a valid identifier.

#### `bool is_valid_identifier_token(const token_t *token)`
Return true if passed token is a valid identifier.

#### `void classify_token(token_t *token)`
Classify the token `token`.

#### `void classify_tokens(token_t *head)`
Starting at token `token`, go through and classify each token in linked list.

