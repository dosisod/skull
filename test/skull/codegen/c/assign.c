#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/assign.h"
#include "skull/parse/ast_node.h"
#include "skull/parse/classify.h"
#include "skull/parse/token.h"
#include "skull/semantic/variable.h"

#include "test/skull/codegen/c/assign.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

static bool test_assign() {
	Token *token = tokenize(U"x = 1");
	classify_tokens(token);

	AstNode *node = AST_NODE_VAR_ASSIGN(
		token,
		AST_SIMPLE_EXPR(token->next->next)
	);
	node->var_assign->expr_node->expr->type = TYPE_INT;
	node->var_assign->expr_node->expr->value._int = 1;
	node->var_assign->var = make_variable(TYPE_INT, U"x", false);

	char *str = var_assign_to_string(node);

	ASSERT_TRUTHY(str);
	ASSERT_EQUAL(strcmp(str, "x = 1;"), 0);

	free(str);
	free_tokens(token);
	free_variable(node->var_assign->var);
	PASS;
}

void codegen_c_assign_test_self(bool *pass) {
	RUN_ALL(
		test_assign
	)
}
