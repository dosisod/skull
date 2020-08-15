# skull/eval/function.c

```c
const char32_t *func_clear(ast_node_t *node)
```

> Returns string that when printed, clears the screen.

```c
const char32_t *func_print(ast_node_t *node, context_t *ctx)
```

> Print out a variable.

```c
bool is_func_name(const ast_node_t *node)
```

> Returns `true` if the given node is a function name.

```c
bool is_func_name_str(const char32_t *name)
```

> Returns `true` if the given string is a function name.

