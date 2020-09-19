# skull/parse/ast/node.c

```c
AstNode *make_ast_tree(const char32_t *code, const char32_t **error)
```

> Makes an AST (abstract syntax tree) from a given string.

```c
AstNode *make_ast_tree_(Token *token, const char32_t **error, unsigned indent_lvl)
```

> Internal AST tree generator.

```c
Token *ast_token_cmp(Token *token, Combo *combo, bool *pass)
```

> Compare tokens against a combonation of tokens.
> \
> Each item in `combo` will be compared with the next token after the last token.
> \
> For example:
> \
> ```c
> ast_token_cmp(token, (Combo[]){
>     { .tok = 0 },
>     { .tok = 1 },
>     { .tok = 2 },
> 	{0}
> });
> ```
> \
> will check up until `token->next->next`.
> \
> The last `{0}` is to tell the function to stop iterating.
> \
> If all the args match, return last token matched, else, the passed `token`.

```c
void push_ast_node(Token *token, Token **last, unsigned node_type, AstNode **node)
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

