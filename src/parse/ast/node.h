#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>

#include "../classify.h"
#include "../tokenize.h"

#define AST_NODE_UNKNOWN 0

typedef struct ast_node_t {
	uint8_t node_type;

	const wchar_t *begin;
	const wchar_t *end;
} ast_node_t;

/*
Makes an ast_node_t with default values
*/
ast_node_t *make_ast_node() {
	ast_node_t *node=malloc(sizeof(ast_node_t));

	node->node_type=AST_NODE_UNKNOWN;
	node->begin=NULL;
	node->end=NULL;

	return node;
}

/*
Returns true if the passed token matches the passed token types.

For example, `ast_token_cmp(token, 0, 1)` will check to see that `token` and `token->next` are of type `0` and `1` respectively.
*/
bool ast_token_cmp(const token_t* token, ...) {
	va_list args;
	va_start(args, token);

	while (token!=NULL) {
		if (token->token_type!=va_arg(args, int)) {
			return false;
		}
		token=token->next;
	}

	va_end(args);
	return true;
}