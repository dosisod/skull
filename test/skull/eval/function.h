#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/function.h"
#include "skull/eval/types/defs.h"

#include "test/testing.h"

TEST(is_func_name, {
	AstNode *node_clear=make_ast_tree(U"clear");
	AstNode *node_print=make_ast_tree(U"print");
	AstNode *node_other=make_ast_tree(U"other");

	const bool pass=(
		is_func_name(node_clear) ||
		is_func_name(node_print) ||
		is_func_name(node_other)
	);

	free(node_clear);
	free(node_print);
	free(node_other);
	return pass;
})

TEST(is_func_name_with_null, {
	return !is_func_name(NULL);
})

TEST(func_clear, {
	AstNode *node=make_ast_tree(U"clear[]");
	const char32_t *str=func_clear(node);

	const bool pass=c32scmp(U"\033[2J\033[;1H", str);

	free((char32_t *)str);
	return pass;
})

TEST(func_clear_params_fail, {
	AstNode *node=make_ast_tree(U"clear[x]");

	const bool pass=c32scmp(
		ERR_UNEXPECTED_PARAM_(U"clear"),
		func_clear(node)
	);

	return pass;
})

TEST(func_print, {
	Variable *var=make_variable("int", U"x", false);
	SkullInt num=1234;
	variable_write(var, &num);
	Context *ctx=make_context();
	context_add_var(ctx, var);

	AstNode *node=make_ast_tree(U"print[x]");
	const char32_t *str=func_print(node, ctx);

	const bool pass=c32scmp(str, U"1234");

	free_ast_tree(node);
	free((char32_t *)str);
	return pass;
})

TEST(func_print_extra_params_fail, {
	Context *ctx=make_context();
	AstNode *node=make_ast_tree(U"print[x, y, z]");

	const bool pass=c32scmp(
		func_print(node, ctx),
		ERR_INVALID_PARAMS_(U"print")
	);

	free_ast_tree(node);
	return pass;
})

TEST(func_print_bad_var, {
	Context *ctx=make_context();
	AstNode *node=make_ast_tree(U"print[x]");

	const bool pass=c32scmp(
		func_print(node, ctx),
		ERR_VAR_NOT_FOUND_(U"x")
	);

	free_ast_tree(node);
	return pass;
})

void function_test_self(bool *pass) {
	tests_t tests={
		test_is_func_name,
		test_is_func_name_with_null,
		test_func_clear,
		test_func_clear_params_fail,
		test_func_print,
		test_func_print_extra_params_fail,
		test_func_print_bad_var,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
