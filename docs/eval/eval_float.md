# src/eval/eval_float.h

#### `long double eval_float(const token_t *token, const wchar_t **error)`
Converts a `TOKEN_FLOAT_CONST` token to a floating point number (`long double`).

Return `NULL` if no errors occurred, else, pointer to error msg.

