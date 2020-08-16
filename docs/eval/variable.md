# skull/eval/variable.c

```c
char32_t *fmt_var(const variable_t *var)
```

> Return string representation of the variable `var`.
> \
> The result of this function must be freed.

```c
void variable_read(void *dest, const variable_t *var)
```

> Read variable memory of `var` into `dest`.

```c
variable_t *make_variable(const char32_t *type, const char32_t *name, bool is_const)
```

> Make a variable called `name` with type `type`, and make it const if `is_const` is true.
> \
> Returns `NULL` if var cannot be created, else pointer to created var.

```c
const char32_t *variable_write(const variable_t *var, const void *data)
```

> Write `data` to `var`.
> \
> If `var` is constant, return error msg, else `NULL`.

```c
void free_variable(variable_t *var)
```

> Free variable `var` and its internal memory.

