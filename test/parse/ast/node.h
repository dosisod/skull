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

void ast_node_test_self(bool *pass) {
	tests_t tests={
		test_ast_node_struct,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
