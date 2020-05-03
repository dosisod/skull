# src/parse/classify.h

#### `bool is_type_token(token_t *token, const char *code)`
Returns true if `token` is a type token.

#### `bool is_keyword_token(token_t *token, const char *code)`
Returns true if `token` is a keyword token.

#### `bool is_operator_token(token_t *token, const char *code)`
Returns true if `token` is an operator token.

#### `bool is_function_token(token_t *token, const char *code)`
Returns true if `token` is a function token.

Function tokens are tokens that look like `name[]`, or `name[`.
They indicate the start of a function declaration.

#### `bool is_function_param_token(token_t *token, const char *code)`
Returns true if `token` is a function parameter.

Function parameters are tokens that look like `name]`, or `name,`.
They indicate that there is a parameter for a given function.

#### `bool is_constant_integer(const char *str)`
Returns true if string is a valid hex/binary/decimal integer.

Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1101`

#### `bool is_constant_integer_token(token_t *token, const char *code)`
Returns true if the passed token is an integer constant.

See above function for examples of valid inputs.

#### `void classify_token(token_t *token, const char *code)`
Classify the token `token`.

#### `void classify_tokens(token_t *head, const char *code)`
Starting at token `token`, go through and classify each token in linked list.

