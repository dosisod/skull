# skull/eval/types/types.c

```c
bool make_new_type(const char32_t *name, size_t bytes)
```

> Creates a new type called `name` with `bytes` bytes of memory.
> \
> Returns false if a type called `name` already exists, and was not inserted.
> Returns true if the type `name` was inserted.

```c
type_t *find_type(const char32_t *name)
```

> Returns pointer to type with name `name`.

```c
void free_types(void)
```

> Free all non-builtin types.
