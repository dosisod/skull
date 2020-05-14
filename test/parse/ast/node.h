#include <stdbool.h>

#include "../src/parse/ast/node.h"

bool test_ast_node_struct() {
	token_t *token=tokenize(L"token");

	ast_node_t node = {
		.token=token,
		.last=NULL,
		.next=NULL
	};

	free(token);

	return (
		node.token!=NULL &&
		node.last==NULL &&
		node.next==NULL
	);
}

bool test_make_ast() {
	token_t *token=tokenize(L"token1");
	ast_node_t *node=make_ast(token);

	bool pass=(
		node->token==token &&
		node->last==NULL &&
		node->next==NULL
	);

	free(token);
	free(node);

	return pass;
}

bool test_make_ast_multi_token() {
	token_t *token=tokenize(L"token1 token2");
	ast_node_t *node=make_ast(token);

	bool pass=(
		node->last==NULL &&
		node->token==token &&
		node->next!=NULL &&
		node->next->token==token->next &&
		node->next->last==node
	);

	free(token->next);
	free(node->next);
	free(token);
	free(node);

	return pass;
}

bool test_free_ast_tree() {
	token_t *token=tokenize(L"token1 token2");
	ast_node_t *node=make_ast(token);

	ast_node_t *node_next=node->next;

	free_ast_tree(node);

	return (
		node->last==NULL &&
		node->token==NULL &&
		node->next==NULL &&
		node_next->last==NULL &&
		node_next->token==NULL &&
		node_next->next==NULL
	);
}

void ast_node_test_self(bool *pass) {
	tests_t tests={
		test_ast_node_struct,
		test_make_ast,
		test_make_ast_multi_token,
		test_free_ast_tree,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
