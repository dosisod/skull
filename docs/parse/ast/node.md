# src/parse/ast/node.h

#### `ast_node_t *make_ast_node()`
Makes an ast_node_t with default values

#### `bool ast_token_cmp(const token_t* token, ...)`
Returns true if the passed token matches the passed token types.

For example, `ast_token_cmp(token, 0, 1)` will check to see that `token` and `token->next` are of type `0` and `1` respectively.

