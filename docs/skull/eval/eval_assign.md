# skull/eval/eval_assign.c

```c
const char32_t *eval_assign(Variable *var, AstNode *node, const Context *ctx)
```

> Assign `node` to variable `var`.
> \
> Set `ctx` to allow for assigning variables to other variables.
> \
> Return an error (as a string) if any occured, else `NULL`.

```c
const char32_t *eval_auto_assign(Variable *var, AstNode *node, const Context *ctx)
```

> Evaluate assignment via auto assignment operator.

