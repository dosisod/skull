# skull/parse/ast/node.c

```c
ast_node_t *make_ast_tree(const char32_t *code)
```

> Makes an AST (abstract syntax tree) from a given string.

```c
token_t *ast_token_cmp(token_t *token, combo_t *combo, bool *pass)
```

> Compare tokens against a combonation of tokens.
> \
> Each item in `combo` will be compared with the next token after the last token.
> \
> For example:
> \
> ```c
> ast_token_cmp(token, (combo_t[]){
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
void push_ast_node(token_t *token, token_t **last, unsigned node_type, ast_node_t **node)
```

> Push a new AST node to `node` with type `node_type`

```c
ast_node_t *make_ast_node(void)
```

> Makes an ast_node_t with default values

```c
void free_ast_tree(ast_node_t *node)
```

> Frees an AST tree.

