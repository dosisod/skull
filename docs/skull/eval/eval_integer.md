# skull/eval/eval_integer.c

```c
void *eval_integer(const token_t *token, const char32_t **error)
```

> Converts a `TOKEN_INT_CONST` token to an integer pointer (`int64_t *`).
> \
> `error` is `NULL` if no error occurs, else `error` points to error msg.

