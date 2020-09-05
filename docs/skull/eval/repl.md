# skull/eval/repl.c

```c
const char32_t *repl_eval(const char *str_, Context *ctx)
```

> Evaluates a string `str` given context `ctx`, returns result as a string (if any).

```c
const char32_t *repl_make_var(const AstNode *node, Context *ctx, bool is_const)
```

> Make and add a variable from `node` to context `ctx`.
> \
> Added variable will be constant if `is_const` is true.
> \
> Returns pointer to error message if one occurs, else `NULL`.

```c
void repl_loop(FILE *fd, Context *ctx)
```

> Read from `fd`, eval with context `ctx`, and print out result.

```c
char *repl_read(FILE *fd)
```

> Read from `fd`, add colored prompt if reading from `stdin`.

