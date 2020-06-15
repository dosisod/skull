#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>

#include "../classify.h"
#include "../tokenize.h"
#include "../../common/malloc.h"

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
} ast_node_t;

/*
Makes an ast_node_t with default values
*/
ast_node_t *make_ast_node(void) {
	ast_node_t *node=malloc(sizeof(ast_node_t));
	DIE_IF_MALLOC_FAILS(node);

	node->node_type=AST_NODE_UNKNOWN;
	node->token=NULL;
	node->token_end=NULL;
	node->next=NULL;

	return node;
}

/*
Compare tokens agains a variable amount of token types (`...`)

Each additional argument will be compared with the next token after the last token.

For example, `ast_token_cmp(token, 0, 1, 2, -1)` will check up until `token->next->next`.

The last `-1` is to tell the function to stop iterating.

If all the args match, return last token matched, else, the passed `token`.
*/
token_t *ast_token_cmp(token_t *token, ...) {
	token_t *head=token;
	token_t *last=head;

	va_list args;
	va_start(args, token);

	int token_type=va_arg(args, int);

	while (token!=NULL && token_type!=-1) {
		if (token->token_type!=token_type && token_type!=TOKEN_ANY_NON_BRACKET_TOKEN) {
			return head;
		}
		if (token_type==TOKEN_ANY_NON_BRACKET_TOKEN &&
			(token->token_type==TOKEN_BRACKET_OPEN ||
			token->token_type==TOKEN_BRACKET_CLOSE))
		{
			return head;
		}
		token_type=va_arg(args, int);
		last=token;
		token=token->next;
	}

	va_end(args);

	if (token_type==-1) {
		return last;
	}
	return head;
}

/*
Push a new AST node to `node` with type `node_type`
*/
void push_ast_node(token_t *token, token_t **last, uint8_t node_type, ast_node_t **node) {
	(*node)->node_type=node_type;
	(*node)->token=(*last);
	(*node)->token_end=token;

	ast_node_t *tmp=make_ast_node();
	(*node)->next=tmp;
	(*node)=tmp;

	*last=token;
}

#define PUSH_AST_NODE_IF(token, last, node_type, node) \
if (token!=*last) { \
	push_ast_node(token, last, node_type, node); \
	continue; \
} \

/*
Makes an AST (abstract syntax tree) from a given string.
*/
ast_node_t *make_ast_tree(const wchar_t *code) {
	token_t *token=tokenize(code);
	token_t *last;

	classify_tokens(token);

	ast_node_t *node=make_ast_node();
	ast_node_t *head=node;

	while (token!=NULL) {
		last=token;
		token=ast_token_cmp(token,
			TOKEN_NEW_IDENTIFIER,
			TOKEN_TYPE,
			TOKEN_OPER_EQUAL, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_VAR_DEF, &node);

		token=ast_token_cmp(token,
			TOKEN_KW_MUT,
			TOKEN_NEW_IDENTIFIER,
			TOKEN_TYPE,
			TOKEN_OPER_EQUAL, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_MUT_VAR_DEF, &node);

		token=ast_token_cmp(token,
			TOKEN_IDENTIFIER,
			TOKEN_OPER_AUTO_EQUAL, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_AUTO_VAR_DEF, &node);

		token=ast_token_cmp(token,
			TOKEN_KW_MUT,
			TOKEN_IDENTIFIER,
			TOKEN_OPER_AUTO_EQUAL, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_MUT_AUTO_VAR_DEF, &node);

		token=ast_token_cmp(token,
			TOKEN_IDENTIFIER,
			TOKEN_OPER_EQUAL, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_VAR_ASSIGN, &node);

		token=ast_token_cmp(token,
			TOKEN_IDENTIFIER,
			TOKEN_OPER_PLUS,
			TOKEN_IDENTIFIER, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_ADD_VAR, &node);

		token=ast_token_cmp(token,
			TOKEN_KW_RETURN,
			TOKEN_INT_CONST, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_RETURN, &node);

		token=ast_token_cmp(token,
			TOKEN_IDENTIFIER,
			TOKEN_BRACKET_OPEN,
			TOKEN_BRACKET_CLOSE, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_NO_PARAM_FUNC, &node);

		token=ast_token_cmp(token,
			TOKEN_IDENTIFIER,
			TOKEN_BRACKET_OPEN,
			TOKEN_ANY_NON_BRACKET_TOKEN,
			TOKEN_BRACKET_CLOSE, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_ONE_PARAM_FUNC, &node);

		if (token->token_type==TOKEN_INT_CONST) {
			push_ast_node(token, &last, AST_NODE_INT_CONST, &node);
		}
		if (token->token_type==TOKEN_FLOAT_CONST) {
			push_ast_node(token, &last, AST_NODE_FLOAT_CONST, &node);
		}
		if (token->token_type==TOKEN_BOOL_CONST) {
			push_ast_node(token, &last, AST_NODE_BOOL_CONST, &node);
		}
		if (token->token_type==TOKEN_CHAR_CONST) {
			push_ast_node(token, &last, AST_NODE_CHAR_CONST, &node);
		}
		if (token->token_type==TOKEN_STR_CONST) {
			push_ast_node(token, &last, AST_NODE_STR_CONST, &node);
		}

		token=token->next;
	}

	return head;
}

/*
Frees an AST tree.
*/
void free_ast_tree(ast_node_t *node) {
	free_tokens(node->token);
	free(node);
}