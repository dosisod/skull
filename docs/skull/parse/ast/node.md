# skull/parse/ast/node

```c
AstNode *make_ast_tree(const char32_t *const code)
```

> Makes an AST (abstract syntax tree) from a given string.

```c
AstNode *make_ast_tree_(Token *token, unsigned indent_lvl)
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

```c
void free_ast_tree_(AstNode *node)
```

> Internal AST freeing function, dont call directly.

```c
bool ast_token_cmp(Token *token, ...)
```

> Check each token's type starting at `token`, checking against the corresponding token type specified in `...`

