# src/parse/ast/node.h

#### `ast_node_t *make_ast_node(void)`
Makes an ast_node_t with default values

#### `token_t *ast_token_cmp(token_t *token, ...)`
Compare tokens agains a variable amount of token types (`...`)

Each additional argument will be compared with the next token after the last token.

For example, `ast_token_cmp(token, 0, 1, 2, -1)` will check up until `token->next->next`.

The last `-1` is to tell the function to stop iterating.

If all the args match, return last token matched, else, the passed `token`.

#### `void push_ast_node(token_t *token, token_t **last, uint8_t node_type, ast_node_t **node)`
Push a new AST node to `node` with type `node_type`

#### `ast_node_t *make_ast_tree(const char32_t *code)`
Makes an AST (abstract syntax tree) from a given string.

#### `void free_ast_tree(ast_node_t *node)`
Frees an AST tree.

