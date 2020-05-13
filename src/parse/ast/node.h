#pragma once

typedef struct ast_node_t {
	struct token_t *token;

	struct ast_node_t *last;
	struct ast_node_t *next;
} ast_node_t;
