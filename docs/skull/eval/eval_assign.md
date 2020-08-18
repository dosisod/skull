# skull/eval/eval_assign.c

```c
const char32_t *eval_assign(variable_t *var, ast_node_t *node, const context_t *ctx)
```

> Assign `node` to variable `var`.
> \
> Set `ctx` to allow for assigning variables to other variables.
> \
> Return an error (as a string) if any occured, else `NULL`.

```c
const char32_t *eval_auto_assign(variable_t *var, ast_node_t *node, const context_t *ctx)
```

> Evaluate assignment via auto assignment operator.
