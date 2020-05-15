#include <stdbool.h>

#include "../src/parse/ast/node.h"

bool test_ast_node_struct() {
	ast_node_t node = {
		.last=NULL,
		.next=NULL,
		.parent=NULL,
		.child=NULL
	};

	return (
		node.last==NULL &&
		node.next==NULL &&
		node.parent==NULL &&
		node.child==NULL
	);
}

bool test_ast_node_single_node() {
	const wchar_t *code=L"hello";
	ast_node_t *node=make_ast_tree(code);

	const bool pass=(
		node->last==NULL &&
		node->next==NULL &&
		node->child==NULL &&
		node->parent==NULL &&
		node->begin==&code[0] &&
		node->end==&code[5]
	);

	free(node);

	return pass;
}

bool test_ast_node_multi_node() {
	const wchar_t *code=L"hello[world[abc";
	ast_node_t *node=make_ast_tree(code);

	const bool pass=(
		node->last==NULL &&
		node->next==NULL &&
		node->child!=NULL &&
		node->parent==NULL &&
		node->begin==&code[0] &&
		node->end==&code[5] &&

		node->child->last==NULL &&
		node->child->next==NULL &&
		node->child->child!=NULL &&
		node->child->parent==node &&
		node->child->begin==&code[6] &&
		node->child->end==&code[11] &&

		node->child->child->last==NULL &&
		node->child->child->next==NULL &&
		node->child->child->child==NULL &&
		node->child->child->parent==node->child &&
		node->child->child->begin==&code[12] &&
		node->child->child->end==&code[15]
	);

	free(node->child->child);
	free(node->child);
	free(node);

	return pass;
}

bool test_ast_node_close_bracket() {
	const wchar_t *code=L"hello[world]";
	ast_node_t *node=make_ast_tree(code);

	const bool pass=(
		node->last==NULL &&
		node->next==NULL &&
		node->child!=NULL &&
		node->parent==NULL &&
		node->begin==&code[0] &&
		node->end==&code[5] &&

		node->child->last==NULL &&
		node->child->next==NULL &&
		node->child->child==NULL &&
		node->child->parent==node &&
		node->child->begin==&code[6] &&
		node->child->end==&code[10]
	);

	free(node->child);
	free(node);

	return pass;
}

void ast_node_test_self(bool *pass) {
	tests_t tests={
		test_ast_node_struct,
		test_ast_node_single_node,
		test_ast_node_multi_node,
		test_ast_node_close_bracket,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
