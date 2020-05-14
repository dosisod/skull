# src/parse/ast/node.h

#### `ast_node_t *make_ast(token_t *token)`
Returns an AST tree built from a linked list of tokens.

#### `void free_ast_tree(ast_node_t *node)`
Frees all nodes in an AST tree, starting from `node`.

