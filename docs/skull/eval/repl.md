# skull/eval/repl.c

```c
const char32_t *repl_make_var(const AstNode *node, Scope *scope, bool is_const)
```

> Make and add a variable from `node` to `scope`.
> \
> Added variable will be constant if `is_const` is true.
> \
> Returns pointer to error message if one occurs, else `NULL`.

