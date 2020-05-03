# src/eval/eval_if.h

#### `int eval_if_true(token_t *token, const char *code)`
Returns `EVAL_IF_TRUE` or `EVAL_IF_FALSE` if token is `"true"` or `"false"`.

If the token isnt `"true"` or `"false"`, `EVAL_TOKEN_ERROR` is returned instead.

#### `int eval_if_equal(token_t *token, const char *code)`
Returns an `EVAL_IF` value for an equality comparison.

If there is an `EVAL_IF_ERROR` on the left or right hand side, `EVAL_IF_ERROR` is returned.

Examples include:

```
"true == false"
"true != true"
```

#### `int eval_if(const char *code)`
Evaluates an if statement in string form.

Examples include:

```
"true"
"false"
"true == false"
"false != false"
"not true"
```

