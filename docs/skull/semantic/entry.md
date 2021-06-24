# skull/semantic/entry

```c
bool validate_ast_tree(const AstNode *node)
```

> Validate an entire AST tree starting at `node`.

```c
static bool _validate_ast_tree(const AstNode *node)
```

> Validate an entire AST tree starting at `node`.

```c
static bool validate_ast_node(const AstNode *node)
```

> Validate a single AST `node`.

```c
static bool state_add_alias(Type type, char *const alias)
```

> Add named `alias` for `type`.
> \
> Return `true` if alias was added, `false` if it already exists.

