# skull/compiler/types/types

```c
typedef const char * Type
```

> Types are simply named strings.

```c
Type __attribute__((pure)) find_type(const char *const name)
```

> Returns pointer to type with name `name`.

```c
bool add_alias(Type type, char *const alias)
```

> Add named `alias` for `type`.
> \
> Return `true` if alias was added, `false` if it already exists.

