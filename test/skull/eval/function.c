#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/function.h"
#include "skull/eval/types/defs.h"

#include "test/skull/eval/function.h"
#include "test/testing.h"

TEST(is_func_name, {
	const char32_t *error = NULL;
	AstNode *node_clear = make_ast_tree(U"clear", &error);
	AstNode *node_print = make_ast_tree(U"print", &error);
	AstNode *node_other = make_ast_tree(U"other", &error);

	ASSERT_FALSEY(error);
	ASSERT_TRUTHY(is_func_name(node_clear));
	ASSERT_TRUTHY(is_func_name(node_print));
	ASSERT_FALSEY(is_func_name(node_other));

	free(node_clear);
	free(node_print);
	free(node_other);
})

TEST(is_func_name_with_null, {
	ASSERT_FALSEY(is_func_name(NULL));
})

TEST(func_clear, {
	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"clear[]", &error);

	const char32_t *str = func_clear(node);

	ASSERT_FALSEY(error);
	ASSERT_TRUTHY(c32scmp(U"\033[2J\033[;1H", str));

	free((char32_t *)str);
})

TEST(func_clear_params_fail, {
	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"clear[x]", &error);

	ASSERT_FALSEY(error);
	ASSERT_TRUTHY(c32scmp(
		ERR_UNEXPECTED_PARAM_(U"clear"),
		func_clear(node)
	));
})

TEST(func_print, {
	Variable *var = make_variable("int", U"x", false);
	SkullInt num = 1234;
	variable_write(var, &num);
	Scope *scope = make_scope();
	scope_add_var(scope, var);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"print[x]", &error);
	const char32_t *str = func_print(node, scope);

	ASSERT_FALSEY(error);
	ASSERT_TRUTHY(c32scmp(str, U"1234"));

	free_ast_tree(node);
	free((char32_t *)str);
})

TEST(func_print_extra_params_fail, {
	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"print[x, z]", &error);

	ASSERT_FALSEY(node);
	ASSERT_TRUTHY(c32scmp(
		error,
		ERR_UNEXPECTED_TOKEN_(U"x,")
	));

	free_ast_tree(node);
})

TEST(func_print_bad_var, {
	Scope *scope = make_scope();
	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"print[x]", &error);

	ASSERT_FALSEY(error);
	ASSERT_TRUTHY(c32scmp(
		func_print(node, scope),
		ERR_VAR_NOT_FOUND_(U"x")
	));

	free_ast_tree(node);
})

TEST(free_ast_node_with_null, {
	free_ast_tree(NULL);
})

TEST_SELF(function,
	test_is_func_name,
	test_is_func_name_with_null,
	test_func_clear,
	test_func_clear_params_fail,
	test_func_print,
	test_func_print_extra_params_fail,
	test_func_print_bad_var,
	test_free_ast_node_with_null
)
