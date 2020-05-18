# src/eval/eval_bool.h

#### `uint8_t eval_bool_true(const token_t *token)`
Returns `EVAL_TRUE` or `EVAL_FALSE` if token is `"true"` or `"false"`.

If the token isnt `"true"` or `"false"`, `EVAL_ERROR` is returned instead.

#### `uint8_t eval_equality_comparison(const token_t *token)`
Returns `EVAL_TRUE` or `EVAL_FALSE` if successfull.

If there is an `EVAL_ERROR` on the left or right hand side, `EVAL_ERROR` is returned.

Examples include:

```
"true == false"
"true != true"
"123 == 123"
"3.14 == 3.14"
```

#### `uint8_t eval_bool(const wchar_t *code)`
Evaluates an boolean expression from string.

Examples include:

```
"true"
"false"
"true == false"
"false != false"
"not true"
```

