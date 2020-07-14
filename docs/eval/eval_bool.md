# src/eval/eval_bool.c

#### `bool eval_bool(const token_t *token, const char32_t **error)`
Resolve a boolean expression from `token` into `true` or `false`.

Examples include:

```
"true"
"false"
"true == false"
"false != false"
"not true"
```

`error` is set to `NULL` if no error occurs, else `error` points to error msg.

#### `bool eval_equality_comparison(const token_t *token, const char32_t **error)`
Return the result of an equality comparison pointed to by `token`.

Examples include:

```
"true == false"
"true != true"
"123 == 123"
"3.14 == 3.14"
```

`error` is set to `NULL` if no error occurs, else `error` points to error msg.

#### `bool eval_bool_true(const token_t *token, const char32_t **error)`
Returns `true` or `false` if `token` is `"true"` or `"false"`.

`error` is set to `NULL` if no error occurs, else `error` points to error msg.

