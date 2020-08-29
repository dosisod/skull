#pragma once

#include <stdint.h>

#include "skull/parse/tokenize.h"

#define TOKEN_ANY_NON_BRACKET_TOKEN (TOKEN_END + 1)

enum node_types {
	AST_NODE_UNKNOWN,

	AST_NODE_IDENTIFIER,

	AST_NODE_VAR_DEF,
	AST_NODE_MUT_VAR_DEF,
	AST_NODE_AUTO_VAR_DEF,
	AST_NODE_MUT_AUTO_VAR_DEF,

	AST_NODE_VAR_ASSIGN,

	AST_NODE_ADD_VAR,
	AST_NODE_SUB_VAR,
	AST_NODE_MULT_VAR,
	AST_NODE_DIV_VAR,

	AST_NODE_RETURN,

	AST_NODE_IF,

	AST_NODE_NO_PARAM_FUNC,
	AST_NODE_ONE_PARAM_FUNC,

	AST_NODE_INT_CONST,
	AST_NODE_FLOAT_CONST,
	AST_NODE_BOOL_CONST,
	AST_NODE_RUNE_CONST,
	AST_NODE_STR_CONST,
	AST_NODE_TYPE_CONST,

	AST_NODE_COMMENT
};

typedef struct ast_node_t {
	uint8_t node_type;

	struct token_t *token;
	struct token_t *token_end;

	struct ast_node_t *next;
	struct ast_node_t *last;
} ast_node_t;

ast_node_t *make_ast_node(void);

token_t *ast_token_cmp(token_t *, int *);
void push_ast_node(token_t *, token_t **, uint8_t, ast_node_t **);

ast_node_t *make_ast_tree(const char32_t *);
void free_ast_tree(ast_node_t *);