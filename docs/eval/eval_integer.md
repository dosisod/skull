# src/eval/eval_integer.h

#### `int64_t eval_integer(const token_t *token, const char32_t **error)`
Converts a `TOKEN_INT_CONST` token to an actual integer (`int64_t`).

Return `NULL` if no errors occurred, else, pointer to error msg.

