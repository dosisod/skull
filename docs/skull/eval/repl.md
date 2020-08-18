# skull/eval/repl.c

```c
const char32_t *repl_eval(const char32_t *str, context_t *ctx)
```

> Evaluates a string `str` given context `ctx`, returns result as a string (if any).

```c
const char32_t *repl_make_var(const ast_node_t *node, context_t *ctx, bool is_const)
```

> Make and add a variable from `node` to context `ctx`.
> \
> Added variable will be constant if `is_const` is true.
> \
> Returns pointer to error message if one occurs, else `NULL`.

```c
void repl_loop(FILE *fd, context_t *ctx)
```

> Read from `fd`, eval with context `ctx`, and print out result.

```c
char32_t *repl_read(FILE *fd)
```

> Read from `fd`, add prompt if reading from `stdin`.

```c
char32_t *repl_read_raw(FILE *fd)
```

> Returns pointer to UTF-32 string read from file descriptor `fd`.

