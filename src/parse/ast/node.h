#pragma once

typedef struct ast_node_t {
	struct token_t *token;

	struct ast_node_t *last;
	struct ast_node_t *next;
} ast_node_t;

/*
Returns an AST tree built from a linked list of tokens.
*/
ast_node_t *make_ast(token_t *token) {
	ast_node_t *head=malloc(sizeof(ast_node_t));
	ast_node_t *current=head;

	head->token=token;
	head->last=NULL;
	head->next=NULL;

	ast_node_t *tmp_node;

	while (token->next) {
		tmp_node=malloc(sizeof(ast_node_t));

		tmp_node->token=token->next;
		tmp_node->last=current;
		tmp_node->next=NULL;

		current->next=tmp_node;
		token=token->next;
	}

	return head;
}

/*
Frees all nodes in an AST tree, starting from `node`.
*/
void free_ast_tree(ast_node_t *node) {
	ast_node_t *current=node;
	ast_node_t *tmp=current;

	while (current!=NULL) {
		tmp=current;
		current=current->next;

		free(tmp->token);
		tmp->token->next=NULL;

		free(tmp);
		tmp->token=NULL;
		tmp->last=NULL;
		tmp->next=NULL;
	}
}
