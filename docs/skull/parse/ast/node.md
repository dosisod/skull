# skull/parse/ast/node.c

```c
AstNode *make_ast_tree(const char32_t *const code, const char32_t **error)
```

> Makes an AST (abstract syntax tree) from a given string.

```c
AstNode *make_ast_tree_(Token *token, const char32_t **error, unsigned indent_lvl)
```

> Internal AST tree generator.

```c
void push_ast_node(Token *const token, Token **last, NodeType node_type, AstNode **node)
```

> Push a new AST node to `node` with type `node_type`

```c
AstNode *make_ast_node(void)
```

> Makes an AstNode with default values

```c
void free_ast_tree(AstNode *node)
```

> Frees an AST tree.

