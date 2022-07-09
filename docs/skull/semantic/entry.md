# skull/semantic/entry

```c
bool validate_ast_tree(SemanticState *state, AstNode *node)
```

> Validate an entire AST tree starting at `node` (the root).

```c
static bool validate_ast_sub_tree(SemanticState *state, AstNode *node)
```

> Validate an AST tree starting at `node`.

```c
static bool validate_ast_node(SemanticState *state, AstNode *node)
```

> Validate a single AST `node`.

