#include <stdbool.h>

#include "../../../src/parse/ast/node.h"
#include "../../../test/testing.h"

bool test_make_ast_node_struct() {
	const wchar_t *code=L"hello";

	ast_node_t node = {
		.node_type=AST_NODE_UNKNOWN,
		.begin=code,
		.end=(code + 4),
		.next=NULL
	};

	return (
		node.node_type==AST_NODE_UNKNOWN &&
		node.begin==code &&
		node.end==(code + 4) &&
		node.next==NULL
	);
}

bool test_make_ast_node() {
	ast_node_t *node=make_ast_node();

	const bool pass=(
		node->node_type==AST_NODE_UNKNOWN &&
		node->begin==NULL &&
		node->end==NULL &&
		node->next==NULL
	);

	free(node);

	return pass;
}

bool test_ast_token_cmp() {
	const wchar_t *code=L"x: int = 0";
	token_t *token=tokenize(code);
	make_default_types();
	classify_tokens(token);

	const bool pass=ast_token_cmp(
		token,
		TOKEN_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPERATOR,
		TOKEN_INT_CONST,
		-1
	)==token->next->next->next;

	free_types();
	free_tokens(token);

	return pass;
}

bool test_ast_token_cmp_extra_tokens() {
	const wchar_t *code=L"x: int = 0 extra";
	token_t *token=tokenize(code);
	make_default_types();
	classify_tokens(token);

	const bool pass=ast_token_cmp(
		token,
		TOKEN_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPERATOR,
		TOKEN_INT_CONST,
		-1
	)==token->next->next->next;

	free_types();
	free_tokens(token);

	return pass;
}

bool test_ast_token_cmp_missing_tokens() {
	const wchar_t *code=L"x: int = 0";
	token_t *token=tokenize(code);
	make_default_types();
	classify_tokens(token);

	const bool pass=ast_token_cmp(
		token,
		TOKEN_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPERATOR,
		TOKEN_INT_CONST,
		TOKEN_UNKNOWN,
		-1
	)==token;

	free_types();
	free_tokens(token);

	return pass;
}

bool test_push_ast_node_if() {
	const wchar_t *code=L"x: int = 0";
	token_t *token=tokenize(code);
	token_t *last=token;

	make_default_types();
	classify_tokens(token);

	ast_node_t *node=make_ast_node();
	ast_node_t *tmp=node;

	token=ast_token_cmp(
		token,
		TOKEN_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPERATOR,
		TOKEN_INT_CONST,
		-1
	);

	push_ast_node_if(token, last, AST_NODE_VAR_DEF, &node);

	const bool pass=(tmp->next==node);

	free_types();
	free_tokens(token);

	return pass;
}

bool test_make_ast_tree_variable_def() {
	const wchar_t *code=L"x: int = 0";
	make_default_types();
	ast_node_t *node=make_ast_tree(code);

	const bool pass=(
		node->node_type==AST_NODE_VAR_DEF &&
		node->begin==code &&
		node->end==(code + 10) &&
		node->next!=NULL
	);

	free_types();
	free(node);

	return pass;
}

bool test_make_ast_tree_many_lines() {
	const wchar_t *code=L"x: int = 0\ny: int = 0";
	make_default_types();
	ast_node_t *node=make_ast_tree(code);

	const bool pass=(
		node->node_type==AST_NODE_VAR_DEF &&
		node->begin==code &&
		node->end==(code + 10) &&
		node->next!=NULL &&
		node->next->node_type==AST_NODE_VAR_DEF &&
		node->next->begin==(code + 11) &&
		node->next->end==(code + 21)
	);

	free_types();
	free(node->next);
	free(node);

	return pass;
}

void ast_node_test_self(bool *pass) {
	tests_t tests={
		test_make_ast_node_struct,
		test_make_ast_node,
		test_ast_token_cmp,
		test_ast_token_cmp_extra_tokens,
		test_ast_token_cmp_missing_tokens,
		test_push_ast_node_if,
		test_make_ast_tree_variable_def,
		test_make_ast_tree_many_lines,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
