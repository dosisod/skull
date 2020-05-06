# src/eval/eval_bool.h

#### `int eval_bool_true(token_t *token, const char *code)`
Returns `EVAL_TRUE` or `EVAL_FALSE` if token is `"true"` or `"false"`.

If the token isnt `"true"` or `"false"`, `EVAL_ERROR` is returned instead.

#### `int eval_equality_comparison(token_t *token, const char *code)`
Returns `EVAL_TRUE` or `EVAL_FALSE` if successfull.

If there is an `EVAL_ERROR` on the left or right hand side, `EVAL_ERROR` is returned.

Examples include:

```
"true == false"
"true != true"
"123 == 123"
"3.14 == 3.14"
```

#### `int eval_bool(const char *code)`
Evaluates an boolean expression from string.

Examples include:

```
"true"
"false"
"true == false"
"false != false"
"not true"
```

