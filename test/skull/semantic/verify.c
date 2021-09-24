#include <stdbool.h>
#include <stdlib.h>

#include "skull/common/errors.h"
#include "skull/parse/ast_node.h"
#include "skull/parse/classify.h"
#include "skull/parse/token.h"
#include "skull/semantic/entry.h"
#include "skull/semantic/expr.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/types.h"

#include "test/testing.h"

#include "./macros.h"


static bool validate_binary_expr_fixture(AstNode *, const char *);
static Token *tokenize_fixture(const char32_t *);

bool test_validate_int_expr() {
	Token *token = tokenize_fixture(U"1");
	AstNode *node = AST_SIMPLE_EXPR(token);

	const bool pass = validate_expr(node);

	free_tokens(token);
	return pass;
}

bool test_validate_int_overflow() {
	Token *token = tokenize_fixture(U"99999999999999999999999999999999");
	AstNode *node = AST_SIMPLE_EXPR(token);

	ASSERT_FALSEY(validate_expr(node));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: overflow occurred while parsing \"99999999999999999999999999999999\"\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_int_underflow() {
	Token *token = tokenize_fixture(U"-99999999999999999999999999999999");
	AstNode *node = AST_SIMPLE_EXPR(token);

	ASSERT_FALSEY(validate_expr(node));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: overflow occurred while parsing \"-99999999999999999999999999999999\"\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_float_overflow() {
	Token *token = tokenize_fixture(U"999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0");
	AstNode *node = AST_SIMPLE_EXPR(token);

	ASSERT_FALSEY(validate_expr(node));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: overflow occurred while parsing \"999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0\"\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_missing_function_decl() {
	Token *token = tokenize_fixture(U"x()");

	AstNode *node = AST_NODE_EXPR(token, AST_NO_ARG_FUNC_EXPR(token));

	ASSERT_FALSEY(validate_expr(node));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: function \"x\" missing declaration\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_trailing_expr() {
	Token *token = tokenize_fixture(U"1");
	AstNode *node = AST_SIMPLE_EXPR(token);

	ASSERT_FALSEY(validate_ast_tree(node));
	free(node->expr->value.str);

	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: expression cannot be used on its own\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_invalid_and() {
	Token *token = tokenize_fixture(U"1 and 1");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_AND);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected type \"Bool\", got \"Int\"\n"
	);
}

bool test_validate_invalid_or() {
	Token *token = tokenize_fixture(U"1 or 1");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_OR);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected type \"Bool\", got \"Int\"\n"
	);
}

bool test_validate_invalid_xor() {
	Token *token = tokenize_fixture(U"1 xor 1");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_XOR);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected type \"Bool\", got \"Int\"\n"
	);
}

bool test_validate_div_by_zero() {
	Token *token = tokenize_fixture(U"1 / 0");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_DIV);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 5: division by zero\n"
	);
}

bool test_validate_lhs_var_missing() {
	Token *token = tokenize_fixture(U"fail + 1");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_BINARY_EXPR(
			AST_IDENT_EXPR(token),
			EXPR_ADD,
			AST_CONST_EXPR(token->next->next)
		)
	);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: variable \"fail\" not found\n"
	);
}

bool test_validate_rhs_var_missing() {
	Token *token = tokenize_fixture(U"1 + fail");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_BINARY_EXPR(
			AST_CONST_EXPR(token),
			EXPR_ADD,
			AST_IDENT_EXPR(token->next->next)
		)
	);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 5: variable \"fail\" not found\n"
	);
}

bool test_validate_pow_type() {
	Token *token = tokenize_fixture(U"\"123\" ^ \"123\"");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_POW);

	const bool pass = validate_binary_expr_fixture(
		node,
		"(null): Compilation error: cannot use type \"Str\" for power operator\n"
	);

	free(node->expr->lhs.expr->value.str);
	free(node->expr->rhs->value.str);

	return pass;
}

bool test_validate_shift_no_ints() {
	Token *token = tokenize_fixture(U"1.0 << 2.0");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_LSHIFT);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected an integer\n"
	);
}

bool test_validate_not_oper_non_bool() {
	Token *token = tokenize_fixture(U"not 1");
	AstNode *node = AST_SIMPLE_UNARY_EXPR(token, EXPR_NOT);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 5: expected type \"Bool\", got \"Int\"\n"
	);
}

bool test_validate_non_numeric_exprs() {
	bool pass = true;

	ExprType *opers = (ExprType[]){
		EXPR_ADD,
		EXPR_SUB,
		EXPR_MULT,
		EXPR_DIV,
		EXPR_MOD,
		EXPR_LESS_THAN,
		EXPR_GTR_THAN,
		EXPR_LESS_THAN_EQ,
		EXPR_GTR_THAN_EQ,
		EXPR_UNKNOWN
	};

	do {
		Token *token = tokenize_fixture(U"true x false");
		AstNode *node = AST_SIMPLE_BINARY_EXPR(token, *opers);

		pass &= validate_binary_expr_fixture(
			node,
			"(null): Compilation error: line 1 column 1: expected a numeric value\n"
		);
	} while (*(++opers));

	return pass;
}

bool test_validate_reassign_non_existent_var() {
	Token *token = tokenize_fixture(U"x = 0");

	AstNode *node = AST_VAR_ASSIGN(
		token,
		AST_NODE_EXPR(token->next->next, AST_CONST_EXPR(token->next->next))
	);

	ASSERT_FALSEY(validate_ast_tree(node));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: variable \"x\" not found\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_check_expr_type_when_declaring() {
	Token *token = tokenize_fixture(U"x := 0\ny: Bool = x");

	Token *var_x_name = token;
	Token *var_x_expr = token->next->next;
	Token *var_y_name = var_x_expr->next->next;
	Token *var_y_expr = var_y_name->next->next->next;

	AstNode *node_x = AST_VAR_DEF(
		var_x_name,
		AST_NODE_EXPR(var_x_expr, AST_CONST_EXPR(var_x_expr)),
		true
	);
	node_x->token_end = token->next;

	AstNode *node_y = AST_VAR_DEF(
		var_y_name,
		AST_NODE_EXPR(var_y_expr, AST_IDENT_EXPR(var_y_expr)),
		false
	);
	node_y->token_end = var_y_name->next->next;

	ASSERT_TRUTHY(validate_ast_tree(node_x));
	ASSERT_FALSEY(validate_ast_tree(node_y));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 2 column 11: expected type \"Bool\", got \"Int\"\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_check_explicit_type_exists() {
	Token *token = tokenize_fixture(U"x: fail = 1");

	AstNode *node = AST_VAR_DEF(
		token,
		AST_NODE_EXPR(token->next->next, AST_CONST_EXPR(token->next->next)),
		false
	);
	node->token_end = token->next;

	ASSERT_FALSEY(validate_ast_tree(node));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 4: type \"fail\" could not be found\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_disallow_reassigning_const() {
	Token *token = tokenize_fixture(U"x := 0\nx = 1");

	AstNode *def = AST_VAR_DEF(
		token,
		AST_NODE_EXPR(token->next->next, AST_CONST_EXPR(token->next->next)),
		true
	);
	def->token_end = token->next;

	Token *assign_name = token->next->next->next->next;
	Token *assign_expr = assign_name->next->next;

	AstNode *assign = AST_VAR_ASSIGN(
		assign_name,
		AST_NODE_EXPR(assign_expr, AST_CONST_EXPR(assign_expr))
	);

	ASSERT_TRUTHY(validate_ast_tree(def));
	ASSERT_FALSEY(validate_ast_tree(assign));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 2 column 1: cannot reassign const variable \"x\"\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_redeclare_function() {
	Token *token = tokenize_fixture(U"external x()");

	AstNode *external_func = AST_NODE_NO_ARGS_FUNC_DECL(
		token->next,
		false,
		false
	);
	ASSERT_TRUTHY(validate_ast_tree(external_func));

	// just call validate again to simulate a redeclaration
	ASSERT_FALSEY(validate_ast_tree(external_func));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 10: cannot redeclare function \"x\"\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_redeclare_variable() {
	Token *token = tokenize_fixture(U"x := 0");

	AstNode *variable = AST_VAR_DEF(
		token,
		AST_NODE_EXPR(
			token->next->next,
			AST_CONST_EXPR(token->next->next)
		),
		true
	);
	variable->token_end = token->next;
	ASSERT_TRUTHY(validate_ast_tree(variable));

	ASSERT_FALSEY(validate_ast_tree(variable));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: variable \"x\" already defined\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_redeclare_variable_as_alias() {
	Token *token = tokenize_fixture(U"x := 1\nx := Int");

	AstNode *variable = AST_VAR_DEF(
		token,
		AST_NODE_EXPR(
			token->next->next,
			AST_CONST_EXPR(token->next->next)
		),
		true
	);
	variable->token_end = token->next;
	ASSERT_TRUTHY(validate_ast_tree(variable));

	Token *alias_token = token->next->next->next->next;

	AstNode *alias = AST_VAR_DEF(
		alias_token,
		AST_NODE_EXPR(
			alias_token->next->next,
			AST_CONST_EXPR(alias_token->next->next)
		),
		true
	);
	alias->token_end = alias_token->next;

	ASSERT_FALSEY(validate_ast_tree(alias));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 2 column 1: cannot redeclare variable \"x\" as type alias\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_else_missing_preceding_if() {
	Token *token = tokenize_fixture(U"else { noop }");

	AstNode *node = &(AstNode){
		.type = AST_NODE_ELSE,
		.token = token,
		.child = &(AstNode){
			.type = AST_NODE_NOOP,
			.token = token->next->next
		},
		.parent = NULL
	};

	ASSERT_FALSEY(validate_ast_tree(node));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: else/elif statement missing preceding if statement\n"
	));

	free_tokens(token);
	PASS
}

void semantic_verify_test_self(bool *pass) {
	RUN_ALL(
		test_validate_int_expr,
		test_validate_int_overflow,
		test_validate_int_underflow,
		test_validate_float_overflow,
		test_validate_missing_function_decl,
		test_validate_trailing_expr,
		test_validate_invalid_and,
		test_validate_invalid_or,
		test_validate_invalid_xor,
		test_validate_div_by_zero,
		test_validate_lhs_var_missing,
		test_validate_rhs_var_missing,
		test_validate_pow_type,
		test_validate_shift_no_ints,
		test_validate_not_oper_non_bool,
		test_validate_non_numeric_exprs,
		test_validate_reassign_non_existent_var,
		test_validate_check_expr_type_when_declaring,
		test_validate_check_explicit_type_exists,
		test_validate_disallow_reassigning_const,
		test_validate_redeclare_function,
		test_validate_redeclare_variable,
		test_validate_redeclare_variable_as_alias,
		test_validate_else_missing_preceding_if
	)
}

static bool validate_binary_expr_fixture(AstNode *node, const char *errors) {
	ASSERT_FALSEY(validate_expr(node));
	ASSERT_FALSEY(compare_errors(errors));

	free_tokens(node->token);
	PASS
}

static Token *tokenize_fixture(const char32_t *code) {
	free_semantic_state();
	Token *token = tokenize(code);
	classify_tokens(token);
	free_errors();

	return token;
}
