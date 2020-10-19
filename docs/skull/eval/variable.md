# skull/eval/variable.c

```c
Variable *make_variable(const Type *const type, const char32_t *const name, bool is_const)
```

> Make a variable called `name` with type `type`, and make it const if `is_const` is true.
> \
> Returns `NULL` if var cannot be created, else pointer to created var.

```c
void free_variable(Variable *var)
```

> Free variable `var`.

