# skull/eval/eval_float.c

```c
void *eval_float(const token_t *token, const char32_t **error)
```

> Converts a `TOKEN_FLOAT_CONST` token to a floating point number pointer (`double *`).
> \
> `error` is `NULL` if no error occurs, else `error` points to error msg.

