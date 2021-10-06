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


#ifndef __ANDROID_API__
#define __ANDROID_API__ 0
#endif

static bool validate_binary_expr_fixture(AstNode *, const char *);
static bool validate_tree_fixture(AstNode *, const char *);
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

bool test_validate_rhs_var_missing() {
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

	return validate_tree_fixture(
		AST_NODE_VAR_ASSIGN(
			token,
			AST_NODE_EXPR(token->next->next, AST_NODE_CONST_EXPR(token->next->next))
		),
		"(null): Compilation error: line 1 column 1: variable \"x\" not found\n"
	);
}

bool test_validate_check_expr_type_when_declaring() {
	Token *token = tokenize_fixture(U"x := 0\ny: Bool = x");

	Token *var_x_expr = token->next->next;
	Token *var_y_name = var_x_expr->next->next;
	Token *var_y_expr = var_y_name->next->next->next;

	AstNode *node_x = AST_NODE_VAR_DEF(
		token,
		AST_NODE_EXPR(var_x_expr, AST_NODE_CONST_EXPR(var_x_expr)),
		true
	);

	AstNode *node_y = AST_NODE_VAR_DEF(
		var_y_name,
		AST_NODE_EXPR(var_y_expr, AST_NODE_IDENT_EXPR(var_y_expr)),
		false
	);

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

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_NODE_EXPR(token->next->next, AST_NODE_CONST_EXPR(token->next->next)),
			false
		),
		"(null): Compilation error: line 1 column 4: type \"fail\" could not be found\n"
	);
}

bool test_validate_disallow_reassigning_const() {
	Token *token = tokenize_fixture(U"x := 0\nx = 1");

	AstNode *def = AST_NODE_VAR_DEF(
		token,
		AST_NODE_EXPR(token->next->next, AST_NODE_CONST_EXPR(token->next->next)),
		true
	);

	Token *assign_name = token->next->next->next->next;
	Token *assign_expr = assign_name->next->next;

	AstNode *assign = AST_NODE_VAR_ASSIGN(
		assign_name,
		AST_NODE_EXPR(assign_expr, AST_NODE_CONST_EXPR(assign_expr))
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

	AstNode *external_func = AST_NODE_NO_ARGS_FUNC_DECL(token, true, false);
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

	AstNode *variable = AST_NODE_VAR_DEF(
		token,
		AST_NODE_EXPR(
			token->next->next,
			AST_NODE_CONST_EXPR(token->next->next)
		),
		true
	);
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

	AstNode *variable = AST_NODE_VAR_DEF(
		token,
		AST_NODE_EXPR(
			token->next->next,
			AST_NODE_CONST_EXPR(token->next->next)
		),
		true
	);
	ASSERT_TRUTHY(validate_ast_tree(variable));

	Token *alias_token = token->next->next->next->next;

	AstNode *alias = AST_NODE_VAR_DEF(
		alias_token,
		AST_NODE_EXPR(
			alias_token->next->next,
			AST_NODE_CONST_EXPR(alias_token->next->next)
		),
		true
	);

	ASSERT_FALSEY(validate_ast_tree(alias));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 2 column 1: cannot redeclare variable \"x\" as type alias\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_else_missing_preceding_if() {
	Token *token = tokenize_fixture(U"else { noop }");

	return validate_tree_fixture(
		AST_NODE_ELSE(token, AST_NODE_NOOP()),
		"(null): Compilation error: line 1 column 1: else/elif statement missing preceding if statement\n"
	);
}

bool test_validate_else_with_comment_missing_preceding_if() {
	Token *token = tokenize_fixture(U"# comment\nelse { noop }");

	AstNode *node = AST_NODE_COMMENT(token);
	node->next = AST_NODE_ELSE(token->next->next, AST_NODE_NOOP());

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: else/elif statement missing preceding if statement\n"
	);
}

bool test_validate_main_return_non_int() {
	Token *token = tokenize_fixture(U"return 1.0");

	return validate_tree_fixture(
		AST_NODE_RETURN(token, AST_SIMPLE_EXPR(token->next)),
		"(null): Compilation error: line 1 column 8: returning non-int expression \"1.0\" from main\n"
	);
}

bool test_validate_check_bool_expr_in_if() {
	Token *token = tokenize_fixture(U"if 0 { noop }");

	return validate_tree_fixture(
		AST_NODE_IF(
			token,
			AST_SIMPLE_EXPR(token->next),
			AST_NODE_NOOP()
		),
		"(null): Compilation error: line 1 column 4: expected boolean expression\n"
	);
}

bool test_validate_check_bool_expr_in_elif() {
	Token *token = tokenize_fixture(U"elif 0 { noop }");

	AstNode *node = AST_NODE_ELIF(
		token,
		AST_SIMPLE_EXPR(token->next),
		AST_NODE_NOOP()
	);
	node->last = AST_NODE_IF(NULL, NULL, NULL);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 1 column 6: expected boolean expression\n"
	);
}

bool test_validate_unreachable_after_return() {
	Token *token = tokenize_fixture(U"return 0\nx := 1");
	Token *var_def_token = token->next->next->next;

	AstNode *node = AST_NODE_RETURN(token, AST_SIMPLE_EXPR(token->next));
	node->next = AST_NODE_VAR_DEF(var_def_token, NULL, false);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: unreachable code\n"
	);
}

bool test_validate_unreachable_code_in_func() {
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

bool test_validate_lhs_var_self_ref() {
	Token *token = tokenize_fixture(U"x := x < 1");
	Token *expr_token = token->next->next;

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_NODE_EXPR(
				expr_token,
				AST_NODE_BINARY_EXPR(
					AST_NODE_IDENT_EXPR(expr_token),
					EXPR_LESS_THAN,
					AST_NODE_CONST_EXPR(expr_token->next->next)
				)
			),
			true
		),
		"(null): Compilation error: line 1 column 6: variable \"x\" not found\n"
	);
}

bool test_validate_reassign_different_type() {
	Token *token = tokenize_fixture(U"mut x := 1\nx = 1.0");
	Token *var_expr_token = token->next->next->next;
	Token *var_assign_token = var_expr_token->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token->next,
		AST_NODE_EXPR(
			var_expr_token,
			AST_NODE_CONST_EXPR(var_expr_token)
		),
		true
	);
	node->var_def->is_const = false;
	node->token = token;

	node->next = AST_NODE_VAR_ASSIGN(
		var_assign_token,
		AST_NODE_EXPR(
			var_assign_token->next->next,
			AST_NODE_CONST_EXPR(var_assign_token->next->next)
		)
	);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 5: expected type \"Int\", got \"Float\"\n"
	);
}

bool test_validate_lhs_expr() {
	Token *token = tokenize_fixture(U"x := y + 1");

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_NODE_EXPR(
				token->next->next,
				AST_NODE_BINARY_EXPR(
					AST_NODE_IDENT_EXPR(token->next->next),
					EXPR_ADD,
					AST_NODE_CONST_EXPR(token->next->next->next->next)
				)
			),
			true
		),
		"(null): Compilation error: line 1 column 6: variable \"y\" not found\n"
	);
}

bool test_validate_rhs_expr() {
	Token *token = tokenize_fixture(U"x := 1.0 + 1");

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_SIMPLE_BINARY_EXPR(token->next->next, EXPR_ADD),
			true
		),
		"(null): Compilation error: line 1 column 12: expected type \"Float\", got \"Int\"\n"
	);
}

bool test_validate_binary_bool_expr() {
	Token *token = tokenize_fixture(U"x: Int = 1 is 1");

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_SIMPLE_BINARY_EXPR(token->next->next->next, EXPR_IS),
			false
		),
		"(null): Compilation error: line 1 column 10: expected type \"Int\", got \"Bool\"\n"
	);
}

bool test_validate_unary_bool_expr() {
	Token *token = tokenize_fixture(U"x: Int = not true");

	return validate_tree_fixture(
		AST_NODE_VAR_DEF(
			token,
			AST_SIMPLE_UNARY_EXPR(token->next->next->next, EXPR_NOT),
			false
		),
		"(null): Compilation error: line 1 column 14: expected type \"Int\", got \"Bool\"\n"
	);
}

bool test_validate_var_in_if_scoped() {
	Token *token = tokenize_fixture(U"if true { x := 0\n}return x");
	Token *cond_token = token->next;
	Token *var_def_token = cond_token->next->next;
	Token *var_def_expr_token = var_def_token->next->next;
	Token *return_token = var_def_expr_token->next->next->next;

	AstNode *node = AST_NODE_IF(
		token,
		AST_NODE_EXPR(
			cond_token,
			AST_NODE_CONST_EXPR(cond_token)
		),
		AST_NODE_VAR_DEF(
			var_def_token,
			AST_NODE_EXPR(
				var_def_expr_token,
				AST_NODE_CONST_EXPR(var_def_expr_token)
			),
			true
		)
	);
	node->next = AST_NODE_RETURN(
		return_token,
		AST_NODE_EXPR(
			return_token->next,
			AST_NODE_IDENT_EXPR(return_token->next)
		)
	);

	return validate_tree_fixture(
		node,
		"(null): Warning: line 1 column 4: condition is always true\n" \
		"(null): Compilation error: line 2 column 9: variable \"x\" not found\n"
	);
}

bool test_validate_stmt_between_if_and_elif() {
	Token *token = tokenize_fixture(U"if true { noop } x := 1\nelif true { noop }");
	Token *if_cond  = token->next;
	Token *var_def_token = if_cond->next->next->next->next;
	Token *var_def_expr_token = var_def_token->next->next;
	Token *elif_token = var_def_expr_token->next->next;

	AstNode *node = AST_NODE_IF(
		token,
		AST_NODE_EXPR(if_cond, AST_NODE_CONST_EXPR(if_cond)),
		AST_NODE_NOOP()
	);
	node->next = AST_NODE_VAR_DEF(
		var_def_token,
		AST_NODE_EXPR(
			var_def_expr_token,
			AST_NODE_CONST_EXPR(var_def_expr_token)
		),
		true
	);
	node->next->next = AST_NODE_ELIF(
		elif_token,
		AST_NODE_EXPR(elif_token->next, AST_NODE_CONST_EXPR(elif_token->next)),
		AST_NODE_NOOP()
	);

	return validate_tree_fixture(
		node,
		"(null): Warning: line 1 column 4: condition is always true\n" \
		"(null): Compilation error: line 2 column 1: else/elif statement missing preceding if statement\n"
	);
}

bool test_validate_no_redeclare_alias() {
	Token *token = tokenize_fixture(U"alias := Int\nalias := Float");
	Token *int_token = token->next->next;
	Token *alias2_token = int_token->next->next;
	Token *float_token = alias2_token->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token,
		AST_NODE_EXPR(int_token, AST_NODE_IDENT_EXPR(int_token)),
		true
	);
	node->next = AST_NODE_VAR_DEF(
		alias2_token,
		AST_NODE_EXPR(float_token, AST_NODE_IDENT_EXPR(float_token)),
		true
	);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: alias \"alias\" is already defined\n"
	);
}

bool test_validate_trivial_type() {
	Token *token = tokenize_fixture(U"x: Int = 1");
	Token *expr_token = token->next->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token,
		AST_NODE_EXPR(expr_token, AST_NODE_CONST_EXPR(expr_token)),
		false
	);
	ASSERT_TRUTHY(validate_ast_tree(node));

	ASSERT_FALSEY(compare_errors(
		"(null): Warning: explicit type \"Int\" can be trivialy deduced\n"
	));

	free_tokens(node->token);
	PASS
}

bool test_validate_no_export_main() {
	Token *token = tokenize_fixture(U"export main() { noop }");

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, false, true);
	node->token = token;

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 1 column 8: cannot export function \"main\"\n"
	);
}

bool test_validate_redeclare_type_alias_as_var() {
	Token *token = tokenize_fixture(U"x := Int\nx := 1");
	Token *type_token = token->next->next;
	Token *var_def_token = type_token->next->next;
	Token *var_def_expr_token = var_def_token->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token,
		AST_NODE_EXPR(type_token, AST_NODE_IDENT_EXPR(type_token)),
		true
	);
	node->next = AST_NODE_VAR_DEF(
		var_def_token,
		AST_NODE_EXPR(var_def_expr_token, AST_NODE_CONST_EXPR(var_def_expr_token)),
		true
	);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: cannot redeclare type alias \"x\" as variable\n"
	);
}

bool test_validate_non_void_function_missing_return() {
	Token *token = tokenize_fixture(U"f() Int { noop }");

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, false, false);
	static char int_name[] = "Int";
	node->func_proto->return_type_name = int_name;

	return validate_tree_fixture(
		node,
		"(null): Compilation error: expected return value in function \"f\"\n"
	);
}

bool test_validate_redeclare_var_as_func() {
	Token *token = tokenize_fixture(U"x := 1\nx() { noop }");
	Token *expr_token = token->next->next;
	Token *func_token = expr_token->next->next;

	AstNode *node = AST_NODE_VAR_DEF(
		token,
		AST_NODE_EXPR(expr_token, AST_NODE_CONST_EXPR(expr_token)),
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
bool test_validate_legal_utf8_str() {
	Token *token = tokenize_fixture(U"\"\\xdb80\"\n");

	return validate_binary_expr_fixture(
		AST_NODE_EXPR(token, AST_NODE_CONST_EXPR(token)),
		"(null): Compilation error: line 1 column 1: illegal UTF8 sequence in this region\n"
	);
}

bool test_validate_legal_utf8_rune() {
	Token *token = tokenize_fixture(U"\'\\xdb80\'\n");

	return validate_binary_expr_fixture(
		AST_NODE_EXPR(token, AST_NODE_CONST_EXPR(token)),
		"(null): Compilation error: line 1 column 1: illegal UTF8 sequence in this region\n"
	);
}
#else
bool test_validate_legal_utf8_str() { PASS; }
bool test_validate_legal_utf8_rune() { PASS; }
#endif

bool test_validate_no_void_assign() {
	Token *token = tokenize_fixture(U"external f()\nx := f()");
	Token *x_token = token->next->next->next->next->next;
	Token *func_call_token = x_token->next->next;

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, true, false);
	node->token = token;

	node->next = AST_NODE_VAR_DEF(
		x_token,
		AST_NODE_EXPR(
			func_call_token,
			AST_NO_ARG_FUNC_EXPR(func_call_token)
		),
		true
	);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 6: function returning type void cannot be assigned to variable \"x\"\n"
	);
}

bool test_validate_return_non_void_from_void_func() {
	Token *token = tokenize_fixture(U"f() { return 0 }");
	Token *return_token = token->next->next->next->next;
	Token *return_expr = return_token->next;

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, false, false);
	node->child = AST_NODE_RETURN(
		return_token,
		AST_NODE_EXPR(return_expr, AST_NODE_CONST_EXPR(return_expr))
	);

	return validate_tree_fixture(
		node,
		"(null): Compilation error: unexpected return from void function \"f\"\n"
	);
}

bool test_validate_unexpected_code_block() {
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

bool test_validate_func_parameter_count() {
	Token *token = tokenize_fixture(U"f() { noop }\nf(1)");
	Token *func_call_token = token->next->next->next->next->next->next->next;
	Token *func_call_expr_token = func_call_token->next->next;

	AstNode *node = AST_NODE_NO_ARGS_FUNC_DECL(token, false, false);
	node->next = AST_NODE_EXPR(
		func_call_token,
		AST_NO_ARG_FUNC_EXPR(func_call_token)
	);

	node->next->expr->lhs.func_call->params = AST_NODE_EXPR(
		func_call_token,
		AST_NODE_CONST_EXPR(func_call_expr_token)
	);
	node->next->expr->lhs.func_call->num_values = 1;

	return validate_tree_fixture(
		node,
		"(null): Compilation error: line 2 column 1: invalid number of parameters\n"
	);
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
		test_validate_else_missing_preceding_if,
		test_validate_else_with_comment_missing_preceding_if,
		test_validate_main_return_non_int,
		test_validate_check_bool_expr_in_if,
		test_validate_check_bool_expr_in_elif,
		test_validate_unreachable_after_return,
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
		test_validate_no_export_main,
		test_validate_redeclare_type_alias_as_var,
		test_validate_non_void_function_missing_return,
		test_validate_redeclare_var_as_func,
		test_validate_no_void_assign,
		test_validate_return_non_void_from_void_func,
		test_validate_legal_utf8_str,
		test_validate_legal_utf8_rune,
		test_validate_unexpected_code_block,
		test_validate_func_parameter_count
	)
}

static bool validate_binary_expr_fixture(AstNode *node, const char *errors) {
	ASSERT_FALSEY(validate_expr(node));
	ASSERT_FALSEY(compare_errors(errors));

	free_tokens(node->token);
	PASS
}

static bool validate_tree_fixture(AstNode *node, const char *errors) {
	ASSERT_FALSEY(validate_ast_tree(node));
	ASSERT_FALSEY(compare_errors(errors));

	free_tokens(node->token);
	PASS
}

static Token *tokenize_fixture(const char32_t *code) {
	free_semantic_state();
	setup_semantic_state();
	Token *token = tokenize(code);
	classify_tokens(token);
	free_errors();

	return token;
}
