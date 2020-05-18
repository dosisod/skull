# src/eval/eval_integer.h

#### `int64_t eval_integer(const token_t *token, uint8_t *error)`
Converts a `TOKEN_INT_CONST` token to an actual integer (`int64_t`).

If an error occurs while converting, `error` is set to `EVAL_INTEGER_ERR`.
`error` should always be `EVAL_INTEGER_OK` upon success.

