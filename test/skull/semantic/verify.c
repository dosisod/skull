#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/parse/ast_node.h"
#include "skull/parse/classify.h"
#include "skull/parse/token.h"
#include "skull/semantic/entry.h"
#include "skull/semantic/expr.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/types.h"

#include "test/testing.h"

#include "./macros.h"


#ifndef __ANDROID_API__
#define __ANDROID_API__ 0 // NOLINT
#endif

static bool validate_binary_expr_fixture(AstNode *, const char *);
static bool validate_tree_fixture(AstNode *, const char *);
static Token *tokenize_fixture(const char32_t *);

static bool test_validate_int_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"1");
	AstNodeExpr *expr = AST_NODE_CONST_EXPR(token);

	const bool pass = validate_expr(state, expr);

	ASSERT_TRUTHY(expr->is_const_expr);
	ASSERT_EQUAL(expr->value._int, 1);

	free_tokens(token);
	free_semantic_state(state);
	return pass;
}

static bool test_validate_float_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"1.0");
	AstNodeExpr *expr = AST_NODE_CONST_EXPR(token);

	const bool pass = validate_expr(state, expr);

	ASSERT_TRUTHY(expr->is_const_expr);
	ASSERT_EQUAL((int)expr->value._float, 1);

	free_tokens(token);
	free_semantic_state(state);
	return pass;
}

static bool test_validate_bool_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"true");
	AstNodeExpr *expr = AST_NODE_CONST_EXPR(token);

	const bool pass = validate_expr(state, expr);

	ASSERT_TRUTHY(expr->is_const_expr);
	ASSERT_TRUTHY(expr->value._bool);

	free_tokens(token);
	free_semantic_state(state);
	return pass;
}

static bool test_validate_rune_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"'x'");
	AstNodeExpr *expr = AST_NODE_CONST_EXPR(token);

	const bool pass = validate_expr(state, expr);

	ASSERT_TRUTHY(expr->is_const_expr);
	ASSERT_EQUAL(expr->value.rune, 'x');

	free_tokens(token);
	free_semantic_state(state);
	return pass;
}

static bool test_validate_str_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"\"x\"");
	AstNodeExpr *expr = AST_NODE_CONST_EXPR(token);

	const bool pass = validate_expr(state, expr);

	ASSERT_TRUTHY(expr->is_const_expr);
	ASSERT_TRUTHY(strcmp(expr->value.str, "x")== 0);

	free_tokens(token);
	free(expr->value.str);
	free_semantic_state(state);
	return pass;
}

static bool test_binary_oper_is_const_expr_if_lhs_and_rhs_are_const_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"1 + 2");
	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		AST_NODE_CONST_EXPR(token),
		EXPR_ADD,
		AST_NODE_CONST_EXPR(token->next->next)
	);

	const bool pass = validate_expr(state, expr);

	ASSERT_TRUTHY(expr->is_const_expr);

	free_tokens(token);
	free_semantic_state(state);
	return pass;
}

static bool test_binary_oper_isnt_const_expr_if_lhs_isnt_const_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"external f() Int\nf() + 2");
	Token *type_token = token->next->next->next->next;
	Token *expr_token = type_token->next->next;;
	Token *int_token = expr_token->next->next->next->next;

	AstNode *func = AST_NODE_NO_ARGS_FUNC_DECL(token, true, false);
	AST_NODE_FUNC_RTYPE(func, type_token);

	ASSERT_TRUTHY(validate_ast_tree(state, func));

	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		AST_NODE_FUNC_EXPR(expr_token),
		EXPR_ADD,
		AST_NODE_CONST_EXPR(int_token)
	);

	const bool pass = validate_expr(state, expr);
	ASSERT_TRUTHY(pass);

	ASSERT_FALSEY(expr->is_const_expr);

	free_tokens(token);
	free_semantic_state(state);
	return pass;
}

static bool test_binary_oper_isnt_const_expr_if_rhs_isnt_const_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"external f() Int\n1 + f()");
	Token *type_token = token->next->next->next->next;
	Token *int_token = type_token->next->next;
	Token *expr_token = int_token->next->next;

	AstNode *func = AST_NODE_NO_ARGS_FUNC_DECL(token, true, false);
	AST_NODE_FUNC_RTYPE(func, type_token);

	ASSERT_TRUTHY(validate_ast_tree(state, func));

	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		AST_NODE_CONST_EXPR(int_token),
		EXPR_ADD,
		AST_NODE_FUNC_EXPR(expr_token)
	);

	const bool pass = validate_expr(state, expr);
	ASSERT_TRUTHY(pass);

	ASSERT_FALSEY(expr->is_const_expr);

	free_tokens(token);
	free_semantic_state(state);
	return pass;
}

static bool test_unary_oper_is_const_expr_if_rhs_is_const_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"not true");

	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		NULL,
		EXPR_NOT,
		AST_NODE_CONST_EXPR(token->next)
	);

	const bool pass = validate_expr(state, expr);

	ASSERT_TRUTHY(expr->is_const_expr);

	free_tokens(token);
	free_semantic_state(state);

	ASSERT_TRUTHY(pass);
	PASS;
}

static bool test_unary_oper_isnt_const_expr_if_rhs_isnt_const_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"external f() Bool\nnot f()");
	Token *type_token = token->next->next->next->next;
	Token *expr_token = type_token->next->next;

	AstNode *func = AST_NODE_NO_ARGS_FUNC_DECL(token, true, false);
	AST_NODE_FUNC_RTYPE(func, type_token);

	ASSERT_TRUTHY(validate_ast_tree(state, func));

	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		NULL,
		EXPR_NOT,
		AST_NODE_FUNC_EXPR(expr_token->next)
	);

	const bool pass = validate_expr(state, expr);
	ASSERT_TRUTHY(pass);

	ASSERT_FALSEY(expr->is_const_expr);

	free_tokens(token);
	free(expr->value.str);
	free_semantic_state(state);
	return pass;
}

static bool test_var_expr_is_const_expr_if_var_def_expr_is_const_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"x := 1\nx");

	AstNode *node = AST_NODE_VAR_DEF(
		token,
		AST_NODE_CONST_EXPR(token->next->next),
		true
	);
	ASSERT_TRUTHY(validate_ast_tree(state, node));

	AstNodeExpr *expr = AST_NODE_IDENT_EXPR(token->next->next->next->next);

	const bool pass = validate_expr(state, expr);
	ASSERT_TRUTHY(expr->is_const_expr);

	free_tokens(token);
	free_semantic_state(state);

	ASSERT_TRUTHY(pass);
	PASS;
}

static bool test_var_expr_isnt_const_expr_if_var_def_expr_isnt_const_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"external f() Int\nx := f()\nx");
	Token *type_token = token->next->next->next->next;
	Token *var_def_token = type_token->next->next;
	Token *var_def_expr_token = var_def_token->next->next;
	Token *expr_token = var_def_expr_token->next->next->next->next;

	AstNode *func = AST_NODE_NO_ARGS_FUNC_DECL(token, true, false);
	AST_NODE_FUNC_RTYPE(func, type_token);

	ASSERT_TRUTHY(validate_ast_tree(state, func));

	AstNodeExpr *func_expr = AST_NODE_FUNC_EXPR(var_def_expr_token);

	AstNode *var_def = AST_NODE_VAR_DEF(var_def_token, func_expr, true);
	ASSERT_TRUTHY(validate_ast_tree(state, var_def));

	AstNodeExpr *expr = AST_NODE_IDENT_EXPR(expr_token);

	const bool pass = validate_expr(state, expr);
	ASSERT_FALSEY(expr->is_const_expr);

	free_tokens(token);
	free_semantic_state(state);

	ASSERT_TRUTHY(pass);
	PASS;
}

static bool test_validate_int_overflow(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"99999999999999999999999999999999");
	AstNodeExpr *expr = AST_NODE_CONST_EXPR(token);

	ASSERT_FALSEY(validate_expr(state, expr));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: overflow occurred while parsing \"99999999999999999999999999999999\"\n"
	));

	free_tokens(token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_int_underflow(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"-99999999999999999999999999999999");
	AstNodeExpr *expr = AST_NODE_CONST_EXPR(token);

	ASSERT_FALSEY(validate_expr(state, expr));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: overflow occurred while parsing \"-99999999999999999999999999999999\"\n"
	));

	free_tokens(token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_float_overflow(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0");
	AstNodeExpr *expr = AST_NODE_CONST_EXPR(token);

	ASSERT_FALSEY(validate_expr(state, expr));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: overflow occurred while parsing \"999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0\"\n"
	));

	free_tokens(token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_missing_function_decl(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"x()");

	AstNode *node = AST_NODE_EXPR(token, AST_NODE_FUNC_EXPR(token));

	ASSERT_FALSEY(validate_expr(state, node->expr));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: function \"x\" missing declaration\n"
	));

	free_tokens(token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_trailing_expr(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"1");
	AstNode *node = AST_NODE_EXPR(token, AST_NODE_CONST_EXPR(token));

	ASSERT_FALSEY(validate_ast_tree(state, node));
	free(node->expr->value.str);

	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: expression cannot be used on its own\n"
	));

	free_tokens(token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_invalid_and(void) {
	Token *token = tokenize_fixture(U"1 and 1");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_AND);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected type \"Bool\", got \"Int\"\n"
	);
}

static bool test_validate_invalid_or(void) {
	Token *token = tokenize_fixture(U"1 or 1");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_OR);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected type \"Bool\", got \"Int\"\n"
	);
}

static bool test_validate_invalid_xor(void) {
	Token *token = tokenize_fixture(U"1 xor 1");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_XOR);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected type \"Bool\", got \"Int\"\n"
	);
}

static bool test_validate_div_by_zero(void) {
	Token *token = tokenize_fixture(U"1 / 0");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_DIV);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 5: division by zero\n"
	);
}

static bool test_validate_lhs_var_missing(void) {
	Token *token = tokenize_fixture(U"fail + 1");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_IDENT_EXPR(token),
			EXPR_ADD,
			AST_NODE_CONST_EXPR(token->next->next)
		)
	);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: variable \"fail\" not found\n"
	);
}

static bool test_validate_rhs_var_missing(void) {
	Token *token = tokenize_fixture(U"1 + fail");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_CONST_EXPR(token),
			EXPR_ADD,
			AST_NODE_IDENT_EXPR(token->next->next)
		)
	);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 5: variable \"fail\" not found\n"
	);
}

static bool test_validate_pow_type(void) {
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

static bool test_validate_shift_no_ints(void) {
	Token *token = tokenize_fixture(U"1.0 << 2.0");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_LSHIFT);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected an integer\n"
	);
}

static bool test_validate_not_oper_non_bool(void) {
	Token *token = tokenize_fixture(U"not 1");
	AstNode *node = AST_SIMPLE_UNARY_EXPR(token, EXPR_NOT);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 5: expected type \"Bool\", got \"Int\"\n"
	);
}

static bool test_validate_non_numeric_exprs(void) {
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

static bool test_validate_reassign_non_existent_var(void) {
	Token *token = tokenize_fixture(U"x = 0");

	return validate_tree_fixture(
		AST_NODE_VAR_ASSIGN(
			token,
			AST_NODE_CONST_EXPR(token->next->next)
		),
		"(null): Compilation error: line 1 column 1: variable \"x\" not found\n"
	);
}

static bool test_validate_check_expr_type_when_declaring(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"x := 0\ny: Bool = x");

	Token *var_x_expr = token->next->next;
	Token *var_y_name = var_x_expr->next->next;
	Token *var_y_expr = var_y_name->next->next->next;

	AstNode *node_x = AST_NODE_VAR_DEF(
		token,
		AST_NODE_CONST_EXPR(var_x_expr),
		true
	);

	AstNode *node_y = AST_NODE_VAR_DEF(
		var_y_name,
		AST_NODE_IDENT_EXPR(var_y_expr),
		false
	);

	ASSERT_TRUTHY(validate_ast_tree(state, node_x));
	ASSERT_FALSEY(validate_ast_tree(state, node_y));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 2 column 11: expected type \"Bool\", got \"Int\"\n"
	));

	free_tokens(token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_check_explicit_type_exists(void) {
	Token *token = tokenize_fixture(U"x: fail = 1");
	Token *expr_token = token->next->next->next;

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_NODE_CONST_EXPR(expr_token),
			false
		),
		"(null): Compilation error: line 1 column 4: type \"fail\" could not be found\n"
	);
}

static bool test_validate_disallow_reassigning_const(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"x := 0\nx = 1");

	AstNode *def = AST_NODE_VAR_DEF(
		token,
		AST_NODE_CONST_EXPR(token->next->next),
		true
	);

	Token *assign_name = token->next->next->next->next;
	Token *assign_expr = assign_name->next->next;

	AstNode *assign = AST_NODE_VAR_ASSIGN(
		assign_name,
		AST_NODE_CONST_EXPR(assign_expr)
	);

	ASSERT_TRUTHY(validate_ast_tree(state, def));
	ASSERT_FALSEY(validate_ast_tree(state, assign));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 2 column 1: cannot reassign const variable \"x\"\n"
	));

	free_tokens(token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_redeclare_function(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"external x()");

	AstNode *external_func = AST_NODE_NO_ARGS_FUNC_DECL(token, true, false);
	ASSERT_TRUTHY(validate_ast_tree(state, external_func));

	// just call validate again to simulate a redeclaration
	ASSERT_FALSEY(validate_ast_tree(state, external_func));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 10: cannot redeclare function \"x\"\n"
	));

	free_tokens(token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_redeclare_variable(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"x := 0");

	AstNode *variable = AST_NODE_VAR_DEF(
		token,
		AST_NODE_CONST_EXPR(token->next->next),
		true
	);
	ASSERT_TRUTHY(validate_ast_tree(state, variable));

	ASSERT_FALSEY(validate_ast_tree(state, variable));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: variable \"x\" already defined\n"
	));

	free_tokens(token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_redeclare_variable_as_alias(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"x := 1\nx := Int");

	AstNode *variable = AST_NODE_VAR_DEF(
		token,
		AST_NODE_CONST_EXPR(token->next->next),
		true
	);
	ASSERT_TRUTHY(validate_ast_tree(state, variable));

	Token *alias_token = token->next->next->next->next;

	AstNode *alias = AST_NODE_VAR_DEF(
		alias_token,
		AST_NODE_IDENT_EXPR(alias_token->next->next),
		true
	);

	ASSERT_FALSEY(validate_ast_tree(state, alias));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 2 column 1: cannot redeclare variable \"x\" as type alias\n"
	));

	free_tokens(token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_else_missing_preceding_if(void) {
	Token *token = tokenize_fixture(U"else { noop }");

	return validate_tree_fixture(
		AST_NODE_ELSE(token, AST_NODE_NOOP()),
		"(null): Compilation error: line 1 column 1: else/elif statement missing preceding if statement\n"
	);
}

static bool test_validate_else_with_comment_missing_preceding_if(void) {
	Token *token = tokenize_fixture(U"# comment\nelse { noop }");

	AstNode *node = AST_NODE_COMMENT(token);
	node->next = AST_NODE_ELSE(token->next->next, AST_NODE_NOOP());

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: else/elif statement missing preceding if statement\n"
	);
}

static bool test_validate_main_return_non_int(void) {
	Token *token = tokenize_fixture(U"return 1.0");

	return validate_tree_fixture(
		AST_NODE_RETURN(token, AST_NODE_CONST_EXPR(token->next)),
		"(null): Compilation error: line 1 column 8: returning non-int expression \"1.0\" from main\n"
	);
}

static bool test_validate_check_bool_expr_in_if(void) {
	Token *token = tokenize_fixture(U"if 0 { noop }");

	return validate_tree_fixture(
		AST_NODE_IF(
			token,
			AST_NODE_CONST_EXPR(token->next),
			AST_NODE_NOOP()
		),
		"(null): Compilation error: line 1 column 4: expected boolean expression\n"
	);
}

static bool test_validate_check_bool_expr_in_elif(void) {
	Token *token = tokenize_fixture(U"elif 0 { noop }");

	AstNode *node = AST_NODE_ELIF(
		token,
		AST_NODE_CONST_EXPR(token->next),
		AST_NODE_NOOP()
	);
	node->last = AST_NODE_IF(NULL, NULL, NULL);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 1 column 6: expected boolean expression\n"
	);
}

static bool test_validate_unreachable_after_return(void) {
	Token *token = tokenize_fixture(U"return 0\nx := 1");
	Token *var_def_token = token->next->next->next;

	AstNode *node = AST_NODE_RETURN(token, AST_NODE_CONST_EXPR(token->next));
	node->next = AST_NODE_VAR_DEF(var_def_token, NULL, false);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: unreachable code\n"
	);
}

static bool test_validate_unreachable_after_break(void) {
	Token *token = tokenize_fixture(U"while true { break\nx := 1\n}");
	Token *break_token = token->next->next->next;
	Token *var_def_token = break_token->next->next;

	AstNode *break_node = AST_NODE_BREAK(break_token);
	break_node->next = AST_NODE_VAR_DEF(var_def_token, NULL, false);

	AstNode *while_node = AST_NODE_WHILE(
		token,
		AST_NODE_CONST_EXPR(token->next),
		break_node
	);

	return validate_tree_fixture(
		while_node,
		"(null): Warning: line 1 column 7: condition is always true\n" \
		"(null): Compilation error: line 2 column 1: unreachable code\n"
	);
}

static bool test_validate_unreachable_after_continue(void) {
	Token *token = tokenize_fixture(U"while true { continue\nx := 1\n}");
	Token *continue_token = token->next->next->next;
	Token *var_def_token = continue_token->next->next;

	AstNode *continue_node = AST_NODE_CONTINUE(continue_token);
	continue_node->next = AST_NODE_VAR_DEF(var_def_token, NULL, false);

	AstNode *while_node = AST_NODE_WHILE(
		token,
		AST_NODE_CONST_EXPR(token->next),
		continue_node
	);

	return validate_tree_fixture(
		while_node,
		"(null): Warning: line 1 column 7: condition is always true\n" \
		"(null): Compilation error: line 2 column 1: unreachable code\n"
	);
}

static bool test_validate_unreachable_code_in_func(void) {
	Token *token = tokenize_fixture(U"f() { return\nx := 1\n}");
	Token *return_token = token->next->next->next->next;
	Token *var_def_token = return_token->next->next;

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, false, false);

	node->child = AST_NODE_RETURN(return_token, NULL);
	node->child->next = AST_NODE_VAR_DEF(var_def_token, NULL, false);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: unreachable code\n"
	);
}

static bool test_validate_lhs_var_self_ref(void) {
	Token *token = tokenize_fixture(U"x := x < 1");
	Token *expr_token = token->next->next;

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_NODE_BINARY_EXPR(
				AST_NODE_IDENT_EXPR(expr_token),
				EXPR_LESS_THAN,
				AST_NODE_CONST_EXPR(expr_token->next->next)
			),
			true
		),
		"(null): Compilation error: line 1 column 6: variable \"x\" not found\n"
	);
}

static bool test_validate_reassign_different_type(void) {
	Token *token = tokenize_fixture(U"mut x := 1\nx = 1.0");
	Token *var_expr_token = token->next->next->next;
	Token *var_assign_token = var_expr_token->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token->next,
		AST_NODE_CONST_EXPR(var_expr_token),
		true
	);
	node->var_def->is_const = false;
	node->token = token;

	node->next = AST_NODE_VAR_ASSIGN(
		var_assign_token,
		AST_NODE_CONST_EXPR(var_assign_token->next->next)
	);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 5: expected type \"Int\", got \"Float\"\n"
	);
}

static bool test_validate_lhs_expr(void) {
	Token *token = tokenize_fixture(U"x := y + 1");

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_NODE_BINARY_EXPR(
				AST_NODE_IDENT_EXPR(token->next->next),
				EXPR_ADD,
				AST_NODE_CONST_EXPR(token->next->next->next->next)
			),
			true
		),
		"(null): Compilation error: line 1 column 6: variable \"y\" not found\n"
	);
}

static bool test_validate_rhs_expr(void) {
	Token *token = tokenize_fixture(U"x := 1.0 + 1");

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_NODE_BINARY_EXPR(
				AST_NODE_CONST_EXPR(token->next->next),
				EXPR_ADD,
				AST_NODE_CONST_EXPR(token->next->next->next->next)
			),
			true
		),
		"(null): Compilation error: line 1 column 12: expected type \"Float\", got \"Int\"\n"
	);
}

static bool test_validate_binary_bool_expr(void) {
	Token *token = tokenize_fixture(U"x: Int = 1 is 1");

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_NODE_BINARY_EXPR(
				AST_NODE_CONST_EXPR(token->next->next->next),
				EXPR_IS,
				AST_NODE_CONST_EXPR(token->next->next->next->next->next)
			),
			false
		),
		"(null): Compilation error: line 1 column 10: expected type \"Int\", got \"Bool\"\n"
	);
}

static bool test_validate_unary_bool_expr(void) {
	Token *token = tokenize_fixture(U"x: Int = not true");

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_NODE_BINARY_EXPR(
				NULL,
				EXPR_NOT,
				AST_NODE_CONST_EXPR(token->next->next->next->next)
			),
			false
		),
		"(null): Compilation error: line 1 column 14: expected type \"Int\", got \"Bool\"\n"
	);
}

static bool test_validate_var_in_if_scoped(void) {
	Token *token = tokenize_fixture(U"if true { x := 0\n}return x");
	Token *cond_token = token->next;
	Token *var_def_token = cond_token->next->next;
	Token *var_def_expr_token = var_def_token->next->next;
	Token *return_token = var_def_expr_token->next->next->next;

	AstNode *node = AST_NODE_IF(
		token,
		AST_NODE_CONST_EXPR(cond_token),
		AST_NODE_VAR_DEF(
			var_def_token,
			AST_NODE_CONST_EXPR(var_def_expr_token),
			true
		)
	);
	node->next = AST_NODE_RETURN(
		return_token,
		AST_NODE_IDENT_EXPR(return_token->next)
	);

	return validate_tree_fixture(
		node,
		"(null): Warning: line 1 column 4: condition is always true\n" \
		"(null): Compilation error: line 2 column 9: variable \"x\" not found\n"
	);
}

static bool test_validate_stmt_between_if_and_elif(void) {
	Token *token = tokenize_fixture(U"if true { noop } x := 1\nelif true { noop }");
	Token *if_cond  = token->next;
	Token *var_def_token = if_cond->next->next->next->next;
	Token *var_def_expr_token = var_def_token->next->next;
	Token *elif_token = var_def_expr_token->next->next;

	AstNode *node = AST_NODE_IF(
		token,
		AST_NODE_CONST_EXPR(if_cond),
		AST_NODE_NOOP()
	);
	node->next = AST_NODE_VAR_DEF(
		var_def_token,
		AST_NODE_CONST_EXPR(var_def_expr_token),
		true
	);
	node->next->next = AST_NODE_ELIF(
		elif_token,
		AST_NODE_CONST_EXPR(elif_token->next),
		AST_NODE_NOOP()
	);

	return validate_tree_fixture(
		node,
		"(null): Warning: line 1 column 4: condition is always true\n" \
		"(null): Compilation error: line 2 column 1: else/elif statement missing preceding if statement\n"
	);
}

static bool test_validate_no_redeclare_alias(void) {
	Token *token = tokenize_fixture(U"alias := Int\nalias := Float");
	Token *int_token = token->next->next;
	Token *alias2_token = int_token->next->next;
	Token *float_token = alias2_token->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token,
		AST_NODE_IDENT_EXPR(int_token),
		true
	);
	node->next = AST_NODE_VAR_DEF(
		alias2_token,
		AST_NODE_IDENT_EXPR(float_token),
		true
	);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: alias \"alias\" is already defined\n"
	);
}

static bool test_validate_trivial_type(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"x: Int = 1");
	Token *expr_token = token->next->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token,
		AST_NODE_CONST_EXPR(expr_token),
		false
	);
	ASSERT_TRUTHY(validate_ast_tree(state, node));

	ASSERT_FALSEY(compare_errors(
		"(null): Warning: explicit type \"Int\" can be trivialy deduced\n"
	));

	free_tokens(node->token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_allow_export_main(void) {
	Token *token = tokenize_fixture(U"export main() { noop }");

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, false, true);
	node->token = token;

	ASSERT_TRUTHY(node);

	free_tokens(token);

	PASS
}

static bool test_validate_redeclare_type_alias_as_var(void) {
	Token *token = tokenize_fixture(U"x := Int\nx := 1");
	Token *type_token = token->next->next;
	Token *var_def_token = type_token->next->next;
	Token *var_def_expr_token = var_def_token->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token,
		AST_NODE_IDENT_EXPR(type_token),
		true
	);
	node->next = AST_NODE_VAR_DEF(
		var_def_token,
		AST_NODE_CONST_EXPR(var_def_expr_token),
		true
	);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: cannot redeclare type alias \"x\" as variable\n"
	);
}

static bool test_validate_non_void_function_missing_return(void) {
	Token *token = tokenize_fixture(U"f() Int { noop }");

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, false, false);
	AST_NODE_FUNC_RTYPE(node, token->next->next->next);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: expected return value in function \"f\"\n"
	);
}

static bool test_validate_redeclare_var_as_func(void) {
	Token *token = tokenize_fixture(U"x := 1\nx() { noop }");
	Token *expr_token = token->next->next;
	Token *func_token = expr_token->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token,
		AST_NODE_CONST_EXPR(expr_token),
		true
	);
	node->next = AST_NODE_NO_ARGS_FUNC_DECL(func_token, false, false);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: cannot redeclare variable \"x\" as function\n"
	);
}

// TODO(dosisod) Investigate why android parses wide chars differently
#ifndef __ANDROID_API__
static bool test_validate_legal_utf8_str(void) {
	Token *token = tokenize_fixture(U"\"\\xdb80\"\n");

	return validate_binary_expr_fixture(
		AST_NODE_EXPR(token, AST_NODE_CONST_EXPR(token)),
		"(null): Compilation error: line 1 column 1: illegal UTF8 sequence in this region\n"
	);
}

static bool test_validate_legal_utf8_rune(void) {
	Token *token = tokenize_fixture(U"\'\\xdb80\'\n");

	return validate_binary_expr_fixture(
		AST_NODE_EXPR(token, AST_NODE_CONST_EXPR(token)),
		"(null): Compilation error: line 1 column 1: illegal UTF8 sequence in this region\n"
	);
}
#else
static bool __attribute__((const)) test_validate_legal_utf8_str(void) { PASS; }
static bool __attribute__((const)) test_validate_legal_utf8_rune(void) { PASS; }
#endif

static bool test_validate_no_void_assign(void) {
	Token *token = tokenize_fixture(U"external f()\nx := f()");
	Token *x_token = token->next->next->next->next->next;
	Token *func_call_token = x_token->next->next;

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, true, false);
	node->token = token;

	AstNodeExpr *func_expr = AST_NODE_FUNC_EXPR(func_call_token);
	node->next = AST_NODE_VAR_DEF(x_token, func_expr, true);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 6: function returning type void cannot be assigned to variable \"x\"\n"
	);
}

static bool test_validate_return_non_void_from_void_func(void) {
	Token *token = tokenize_fixture(U"f() { return 0 }");
	Token *return_token = token->next->next->next->next;
	Token *return_expr = return_token->next;

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, false, false);
	node->child = AST_NODE_RETURN(
		return_token,
		AST_NODE_CONST_EXPR(return_expr)
	);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: unexpected return from void function \"f\"\n"
	);
}

static bool test_validate_unexpected_code_block(void) {
	Token *token = tokenize_fixture(U"noop { noop }");

	AstNode *node = AST_NODE_NOOP();
	node->token = token;
	node->child = AST_NODE_NOOP();
	node->child->token = token->next;

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 1 column 6: unexpected code block\n"
	);
}

static bool test_validate_func_parameter_count(void) {
	Token *token = tokenize_fixture(U"f() { noop }\nf(1)");
	Token *func_call_token = token->next->next->next->next->next->next->next;
	Token *func_call_expr_token = func_call_token->next->next;

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, false, false);
	AstNodeExpr *func_expr = AST_NODE_FUNC_EXPR(func_call_token);
	node->next = AST_NODE_EXPR(func_call_token, func_expr);

	AST_NODE_FUNC_ADD_PARAM(
		node->next->expr,
		AST_NODE_EXPR(func_call_token, AST_NODE_CONST_EXPR(func_call_expr_token))
	);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: invalid number of parameters\n"
	);
}

static bool test_validate_func_return_invalid_type(void) {
	Token *token = tokenize_fixture(U"f() Int { return false }");
	Token *type_token = token->next->next->next;
	Token *return_token = type_token->next->next;
	Token *return_expr_token = return_token->next;

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, false, false);
	AST_NODE_FUNC_RTYPE(node, type_token);

	node->child = AST_NODE_RETURN(
		return_token,
		AST_NODE_CONST_EXPR(return_expr_token)
	);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 1 column 18: expected type \"Int\", got \"Bool\"\n"
	);
}

static bool test_validate_func_check_return_type(void) {
	Token *token = tokenize_fixture(U"f() invalid { noop }");

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, false, false);
	AST_NODE_FUNC_RTYPE(node, token->next->next->next);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 1 column 5: type \"invalid\" could not be found\n"
	);
}

static bool test_validate_redeclare_alias_as_func(void) {
	Token *token = tokenize_fixture(U"x := Int\nx() { noop }");
	Token *var_expr_token = token->next->next;
	Token *func_token = var_expr_token->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token,
		AST_NODE_IDENT_EXPR(var_expr_token),
		true
	);

	node->next = AST_NODE_NO_ARGS_FUNC_DECL(func_token, false, false);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: cannot redeclare type alias \"x\" as function\n"
	);
}

static bool test_break_outside_while(void) {
	Token *token = tokenize_fixture(U"break");

	AstNode *node = AST_NODE_BREAK(token);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 1 column 1: break must be inside while loop\n"
	);
}

static bool test_continue_outside_while(void) {
	Token *token = tokenize_fixture(U"continue");

	AstNode *node = AST_NODE_CONTINUE(token);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 1 column 1: continue must be inside while loop\n"
	);
}

static bool test_validate_is_int(void) {
	SemanticState *state = setup_semantic_state();
	Token *token = tokenize_fixture(U"1 is 1");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_CONST_EXPR(token),
			EXPR_IS,
			AST_NODE_CONST_EXPR(token->next->next)
		)
	);

	ASSERT_TRUTHY(validate_expr(state, node->expr));
	ASSERT_EQUAL(node->expr->type, &TYPE_BOOL);

	free_tokens(token);
	free_semantic_state(state);
	PASS;
}

static bool test_validate_is_float(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"1.0 is 1.0");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_CONST_EXPR(token),
			EXPR_IS,
			AST_NODE_CONST_EXPR(token->next->next)
		)
	);

	ASSERT_TRUTHY(validate_expr(state, node->expr));
	ASSERT_EQUAL(node->expr->type, &TYPE_BOOL);

	free_tokens(token);
	free_semantic_state(state);
	PASS;
}

static bool test_validate_is_rune(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"'x' is 'x'");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_CONST_EXPR(token),
			EXPR_IS,
			AST_NODE_CONST_EXPR(token->next->next)
		)
	);

	ASSERT_TRUTHY(validate_expr(state, node->expr));
	ASSERT_EQUAL(node->expr->type, &TYPE_BOOL);

	free_tokens(token);
	free_semantic_state(state);
	PASS;
}

static bool test_validate_is_bool(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"true is true");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_CONST_EXPR(token),
			EXPR_IS,
			AST_NODE_CONST_EXPR(token->next->next)
		)
	);

	ASSERT_TRUTHY(validate_expr(state, node->expr));
	ASSERT_EQUAL(node->expr->type, &TYPE_BOOL);

	free_tokens(token);
	free_semantic_state(state);
	PASS;
}

static bool test_validate_is_str(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"\"x\" is \"x\"");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_CONST_EXPR(token),
			EXPR_IS,
			AST_NODE_CONST_EXPR(token->next->next)
		)
	);

	ASSERT_TRUTHY(validate_expr(state, node->expr));
	ASSERT_EQUAL(node->expr->type, &TYPE_BOOL);

	free_tokens(token);
	free(node->expr->lhs.expr->value.str);
	free(node->expr->rhs->value.str);
	free_semantic_state(state);
	PASS;
}

static bool test_validate_isnt_int(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"1 isnt 1");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_CONST_EXPR(token),
			EXPR_ISNT,
			AST_NODE_CONST_EXPR(token->next->next)
		)
	);

	ASSERT_TRUTHY(validate_expr(state, node->expr));
	ASSERT_EQUAL(node->expr->type, &TYPE_BOOL);

	free_tokens(token);
	free_semantic_state(state);
	PASS;
}

static bool test_validate_isnt_float(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"1.0 isnt 1.0");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_CONST_EXPR(token),
			EXPR_ISNT,
			AST_NODE_CONST_EXPR(token->next->next)
		)
	);

	ASSERT_TRUTHY(validate_expr(state, node->expr));
	ASSERT_EQUAL(node->expr->type, &TYPE_BOOL);

	free_tokens(token);
	free_semantic_state(state);
	PASS;
}

static bool test_validate_isnt_rune(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"'x' isnt 'x'");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_CONST_EXPR(token),
			EXPR_ISNT,
			AST_NODE_CONST_EXPR(token->next->next)
		)
	);

	ASSERT_TRUTHY(validate_expr(state, node->expr));
	ASSERT_EQUAL(node->expr->type, &TYPE_BOOL);

	free_tokens(token);
	free_semantic_state(state);
	PASS;
}

static bool test_validate_isnt_bool(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"true isnt true");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_CONST_EXPR(token),
			EXPR_ISNT,
			AST_NODE_CONST_EXPR(token->next->next)
		)
	);

	ASSERT_TRUTHY(validate_expr(state, node->expr));
	ASSERT_EQUAL(node->expr->type, &TYPE_BOOL);

	free_tokens(token);
	free_semantic_state(state);
	PASS;
}

static bool test_validate_isnt_str(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"\"x\" isnt \"x\"");

	AstNode *node = AST_NODE_EXPR(
		token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_CONST_EXPR(token),
			EXPR_ISNT,
			AST_NODE_CONST_EXPR(token->next->next)
		)
	);

	ASSERT_TRUTHY(validate_expr(state, node->expr));
	ASSERT_EQUAL(node->expr->type, &TYPE_BOOL);

	free_tokens(token);
	free(node->expr->lhs.expr->value.str);
	free(node->expr->rhs->value.str);
	free_semantic_state(state);
	PASS;
}

static bool test_validate_matching_types(void) {
	ExprType *opers = (ExprType[]){
		EXPR_ADD,
		EXPR_SUB,
		EXPR_MULT,
		EXPR_DIV,
		EXPR_MOD,
		EXPR_LSHIFT,
		EXPR_RSHIFT,
		EXPR_POW,
		EXPR_IS,
		EXPR_ISNT,
		EXPR_LESS_THAN,
		EXPR_GTR_THAN,
		EXPR_LESS_THAN_EQ,
		EXPR_GTR_THAN_EQ,
		EXPR_UNKNOWN
	};

	bool pass = true;

	do {
		Token *token = tokenize_fixture(U"1 x 1.0");
		AstNode *node = AST_SIMPLE_BINARY_EXPR(token, *opers);

		pass &= validate_binary_expr_fixture(
			node,
			"(null): Compilation error: line 1 column 5: expected type \"Int\", got \"Float\"\n"
		);
	} while (*(++opers));

	return pass;
}

static bool test_validate_type_alias_mut_not_allowed(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"mut x := Int");
	Token *int_token = token->next->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token,
		AST_NODE_IDENT_EXPR(int_token),
		true
	);
	node->var_def->is_const = false;
	node->token_end = token->next->next;

	const char *errors = "(null): Compilation error: line 1 column 5: type alias cannot be mutable\n";

	ASSERT_FALSEY(validate_ast_tree(state, node));
	ASSERT_FALSEY(compare_errors(errors));

	free_tokens(node->token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_type_alias_in_expr_not_allowed(void) {
	SemanticState *state = setup_semantic_state();

	Token *token = tokenize_fixture(U"x := Int\ny := x + 1");
	Token *int_token = token->next->next;
	Token *y_token = int_token->next->next;
	Token *x_token = y_token->next->next;
	Token *one_token = x_token->next->next;

	AstNode *x_def = AST_NODE_VAR_DEF(
		token,
		AST_NODE_IDENT_EXPR(int_token),
		true
	);
	ASSERT_TRUTHY(validate_ast_tree(state, x_def));

	AstNode *y_def = AST_NODE_VAR_DEF(
		y_token,
		AST_NODE_BINARY_EXPR(
			AST_NODE_IDENT_EXPR(x_token),
			EXPR_ADD,
			AST_NODE_CONST_EXPR(one_token)
		),
		true
	);

	const char *errors = "(null): Compilation error: line 2 column 6: type aliases cannot be used in expressions\n";

	ASSERT_FALSEY(validate_ast_tree(state, y_def));
	ASSERT_FALSEY(compare_errors(errors));

	free_tokens(x_def->token);
	free_semantic_state(state);
	PASS
}

static bool test_validate_break_in_func_def(void) {
	Token *token = tokenize_fixture(U"while true { f() { break } }");

	Token *func_token = token->next->next->next;
	Token *break_token = func_token->next->next->next->next;

	AstNode *func = AST_NODE_NO_ARGS_FUNC_DECL(func_token, false, false);
	func->child = AST_NODE_BREAK(break_token);

	AstNode *while_node = AST_NODE_WHILE(
		token,
		AST_NODE_CONST_EXPR(token->next),
		func
	);

	return validate_tree_fixture(
		while_node,
		"(null): Warning: line 1 column 7: condition is always true\n" \
		"(null): Compilation error: line 1 column 20: break must be inside while loop\n"
	);
}

static bool test_validate_func_call_on_var_fails(void) {
	Token *token = tokenize_fixture(U"x := 1\nx()");
	Token *expr_token = token->next->next;
	Token *func_token = expr_token->next->next;

	AstNode *var_def = AST_NODE_VAR_DEF(token, AST_NODE_CONST_EXPR(expr_token), true);
	AstNode *func = AST_NODE_EXPR(func_token, AST_NODE_FUNC_EXPR(func_token));

	var_def->next = func;

	return validate_tree_fixture(
		var_def,
		"(null): Compilation error: line 2 column 1: function \"x\" missing declaration\n"
	);
}


void semantic_verify_test_self(bool *pass) {
	RUN_ALL(
		test_validate_int_expr,
		test_validate_float_expr,
		test_validate_bool_expr,
		test_validate_rune_expr,
		test_validate_str_expr,
		test_binary_oper_is_const_expr_if_lhs_and_rhs_are_const_expr,
		test_binary_oper_isnt_const_expr_if_lhs_isnt_const_expr,
		test_binary_oper_isnt_const_expr_if_rhs_isnt_const_expr,
		test_unary_oper_isnt_const_expr_if_rhs_isnt_const_expr,
		test_unary_oper_is_const_expr_if_rhs_is_const_expr,
		test_var_expr_is_const_expr_if_var_def_expr_is_const_expr,
		test_var_expr_isnt_const_expr_if_var_def_expr_isnt_const_expr,
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
		test_validate_else_missing_preceding_if,
		test_validate_else_with_comment_missing_preceding_if,
		test_validate_main_return_non_int,
		test_validate_check_bool_expr_in_if,
		test_validate_check_bool_expr_in_elif,
		test_validate_unreachable_after_return,
		test_validate_unreachable_after_break,
		test_validate_unreachable_after_continue,
		test_validate_unreachable_code_in_func,
		test_validate_lhs_var_self_ref,
		test_validate_reassign_different_type,
		test_validate_lhs_expr,
		test_validate_rhs_expr,
		test_validate_binary_bool_expr,
		test_validate_unary_bool_expr,
		test_validate_var_in_if_scoped,
		test_validate_stmt_between_if_and_elif,
		test_validate_no_redeclare_alias,
		test_validate_trivial_type,
		test_validate_allow_export_main,
		test_validate_redeclare_type_alias_as_var,
		test_validate_non_void_function_missing_return,
		test_validate_redeclare_var_as_func,
		test_validate_no_void_assign,
		test_validate_return_non_void_from_void_func,
		test_validate_legal_utf8_str,
		test_validate_legal_utf8_rune,
		test_validate_unexpected_code_block,
		test_validate_func_parameter_count,
		test_validate_func_return_invalid_type,
		test_validate_func_check_return_type,
		test_validate_redeclare_alias_as_func,
		test_break_outside_while,
		test_continue_outside_while,
		test_validate_is_int,
		test_validate_is_float,
		test_validate_is_rune,
		test_validate_is_bool,
		test_validate_is_str,
		test_validate_isnt_int,
		test_validate_isnt_float,
		test_validate_isnt_rune,
		test_validate_isnt_bool,
		test_validate_isnt_str,
		test_validate_matching_types,
		test_validate_type_alias_mut_not_allowed,
		test_validate_type_alias_in_expr_not_allowed,
		test_validate_break_in_func_def,
		test_validate_func_call_on_var_fails
	)
}

static bool validate_binary_expr_fixture(AstNode *node, const char *errors) {
	SemanticState *state = setup_semantic_state();

	ASSERT_FALSEY(validate_expr(state, node->expr));
	ASSERT_FALSEY(compare_errors(errors));

	free_tokens(node->token);
	free_semantic_state(state);
	PASS
}

static bool validate_tree_fixture(AstNode *node, const char *errors) {
	SemanticState *state = setup_semantic_state();

	ASSERT_FALSEY(validate_ast_tree(state, node));
	ASSERT_FALSEY(compare_errors(errors));

	free_tokens(node->token);
	free_semantic_state(state);
	PASS
}

static Token *tokenize_fixture(const char32_t *code) {
	Token *token = tokenize(code);
	classify_tokens(token);
	free_errors();

	return token;
}
