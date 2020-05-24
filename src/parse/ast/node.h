#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>

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