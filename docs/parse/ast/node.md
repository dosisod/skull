# src/parse/ast/node.h

#### `ast_node_t *make_ast_node()`
Makes an ast_node_t with default values

#### `const token_t *ast_token_cmp(const token_t *token, ...)`
Compare tokens agains a variable amount of token types (`...`)

Each additional argument will be compared with the next token after the last token.

For example, `ast_token_cmp(token, 0, 1, 2, -1)` will check up until `token->next->next`.

The last `-1` is to tell the function to stop iterating.

If all the args match, return last token matched, else, the passed `token`.

