# src/eval/eval_float.h

#### `long double eval_float(const token_t *token, uint8_t *error)`
Converts a `TOKEN_FLOAT_CONST` token to a floating point number (`long double`).

If an error occurs while converting, `error` is set to `EVAL_FLOAT_ERR`.
`error` should always be `EVAL_FLOAT_OK` upon success.

