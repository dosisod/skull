#include <stdbool.h>

#include "../../../src/parse/ast/node.h"
#include "../../../test/testing.h"

bool test_make_ast_node_struct() {
	const wchar_t *code=L"hello";

	ast_node_t node = {
		.node_type=AST_NODE_UNKNOWN,
		.begin=code,
		.end=(code + 4)
	};

	return (
		node.node_type==AST_NODE_UNKNOWN &&
		node.begin==code &&
		node.end==(code + 4)
	);
}

bool test_make_ast_node() {
	ast_node_t *node=make_ast_node();

	const bool pass=(
		node->node_type==AST_NODE_UNKNOWN &&
		node->begin==NULL &&
		node->end==NULL
	);

	free(node);

	return pass;
}

bool test_ast_token_cmp() {
	const wchar_t *code=L"int x = 0";
	token_t *token=tokenize(code);
	make_default_types();
	classify_tokens(token);

	const bool pass=ast_token_cmp(
		token,
		TOKEN_TYPE,
		TOKEN_UNKNOWN,
		TOKEN_OPERATOR,
		TOKEN_INT_CONST
	);

	free_types();
	free_tokens(token);

	return pass;
}

void ast_node_test_self(bool *pass) {
	tests_t tests={
		test_make_ast_node_struct,
		test_make_ast_node,
		test_ast_token_cmp,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
