#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>

#include "../classify.h"
#include "../tokenize.h"

#define AST_NODE_UNKNOWN 0
#define AST_NODE_VAR_DEF 1

typedef struct ast_node_t {
	uint8_t node_type;

	const wchar_t *begin;
	const wchar_t *end;

	struct ast_node_t *next;
} ast_node_t;

/*
Makes an ast_node_t with default values
*/
ast_node_t *make_ast_node() {
	ast_node_t *node=malloc(sizeof(ast_node_t));

	node->node_type=AST_NODE_UNKNOWN;
	node->begin=NULL;
	node->end=NULL;
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
		if (token->token_type!=token_type) {
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
		token=ast_token_cmp(
			token,
			TOKEN_TYPE,
			TOKEN_UNKNOWN,
			TOKEN_OPERATOR,
			TOKEN_INT_CONST,
			-1
		);

		if (token!=last) {
			node->node_type=AST_NODE_VAR_DEF;
			node->begin=last->begin;
			node->end=token->end;

			ast_node_t *tmp=make_ast_node();
			node->next=tmp;
			node=tmp;
		}

		token=token->next;
	}

	free_tokens(token);
	return head;
}