#pragma once

#include <stdint.h>

#include "../tokenize.h"

enum node_types {
	AST_NODE_UNKNOWN,

	AST_NODE_VAR_DEF,
	AST_NODE_MUT_VAR_DEF,
	AST_NODE_AUTO_VAR_DEF,
	AST_NODE_MUT_AUTO_VAR_DEF,

	AST_NODE_VAR_ASSIGN,

	AST_NODE_ADD_VAR,

	AST_NODE_RETURN,

	AST_NODE_NO_PARAM_FUNC,
	AST_NODE_ONE_PARAM_FUNC,

	AST_NODE_INT_CONST,
	AST_NODE_FLOAT_CONST,
	AST_NODE_BOOL_CONST,
	AST_NODE_CHAR_CONST,
	AST_NODE_STR_CONST
};

typedef struct ast_node_t {
	uint8_t node_type;

	struct token_t *token;
	struct token_t *token_end;

	struct ast_node_t *next;
	struct ast_node_t *last;
} ast_node_t;

/*
Makes an ast_node_t with default values
*/
ast_node_t *make_ast_node(void);

/*
Compare tokens agains a variable amount of token types (`...`)

Each additional argument will be compared with the next token after the last token.

For example, `ast_token_cmp(token, 0, 1, 2, -1)` will check up until `token->next->next`.

The last `-1` is to tell the function to stop iterating.

If all the args match, return last token matched, else, the passed `token`.
*/
token_t *ast_token_cmp(token_t *token, ...);

/*
Push a new AST node to `node` with type `node_type`
*/
void push_ast_node(token_t *token, token_t **last, uint8_t node_type, ast_node_t **node);

/*
Makes an AST (abstract syntax tree) from a given string.
*/
ast_node_t *make_ast_tree(const char32_t *code);

/*
Frees an AST tree.
*/
void free_ast_tree(ast_node_t *node);