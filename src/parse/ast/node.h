#pragma once

typedef struct ast_node_t {
	struct ast_node_t *last;
	struct ast_node_t *next;

	struct ast_node_t *parent;
	struct ast_node_t *child;

	const wchar_t *begin;
	const wchar_t *end;
} ast_node_t;

/*
Generate an AST tree from a pointer to `code`.

Nodes represent the areas between `[]` characters.
*/
ast_node_t *make_ast_tree(const wchar_t *code) {
	ast_node_t *head=malloc(sizeof(ast_node_t));

	head->last=NULL;
	head->next=NULL;
	head->child=NULL;
	head->parent=NULL;

	head->begin=code;
	head->end=code;

	while (*code!=L'\0') {
		if (*code==L'[') {
			ast_node_t *tmp=make_ast_tree(code + 1);
			tmp->last=NULL;
			tmp->next=NULL;

			tmp->parent=head;
			head->child=tmp;
			head->end=code;

			return head;
		}
		else if (*code==L']') {
			head->end=code - 1;
			return head;
		}
		code++;
	}

	head->end=code;

	return head;
}
