# src/eval/eval_integer.h

#### `long long int eval_integer(const token_t *token, short int *error)`
Converts a `TOKEN_INT_CONST` token to an actual integer (long long int).

If an error occurs while converting, `error` is set to `EVAL_INTEGER_ERR`.
`error` should always be `EVAL_INTEGER_OK` upon success.

