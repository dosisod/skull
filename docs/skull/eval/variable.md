# skull/eval/variable.c

```c
char32_t *fmt_var(const Variable *const var)
```

> Return string representation of the variable `var`.
> \
> The result of this function must be freed.

```c
void variable_read(void *const dest, const Variable *const var)
```

> Read variable memory of `var` into `dest`.

```c
Variable *make_variable(const Type *const type, const char32_t *const name, bool is_const)
```

> Make a variable called `name` with type `type`, and make it const if `is_const` is true.
> \
> Returns `NULL` if var cannot be created, else pointer to created var.

```c
const char32_t *variable_write(const Variable *const var, const void *const data)
```

> Write `data` to `var`.
> \
> If `var` is constant, return error msg, else `NULL`.

```c
void free_variable(Variable *var)
```

> Free variable `var` and its internal memory.

