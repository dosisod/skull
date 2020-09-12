# skull/eval/function.c

```c
const char32_t *func_clear(AstNode *node)
```

> Returns string that when printed, clears the screen.

```c
const char32_t *func_print(AstNode *node, Scope *scope)
```

> Print out a variable.

```c
bool is_func_name(const AstNode *node)
```

> Returns `true` if the given node is a function name.

```c
bool is_func_name_str(const char32_t *name)
```

> Returns `true` if the given string is a function name.

