#include <stdbool.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/common/errors.h"
#include "skull/eval/eval_assign.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/variable.h"
#include "skull/parse/classify.h"

#include "test/skull/eval/variable.h"
#include "test/testing.h"

TEST(create_variable, {
	Variable *var = make_variable(&TYPE_INT, U"x", true);

	ASSERT_EQUAL(strcmp(var->type->name, *&TYPE_INT.name), 0);
	ASSERT_TRUTHY(c32scmp(var->name, U"x"));
	ASSERT_TRUTHY(var->is_const);
	ASSERT_EQUAL(var->type->bytes, 8);
	ASSERT_FALSEY(var->mem);
	ASSERT_FALSEY(var->alloca);

	free_variable(var);
})

TEST(create_variable_with_invalid_type_fails, {
	ASSERT_FALSEY(make_variable(NULL, U"x", true));
})

TEST(variable_write, {
	Variable *var = make_variable(&TYPE_INT, U"x", false);

	const SkullInt data = 1234;
	char32_t *err = variable_write(var, &data);

	SkullInt val = 0;
	memcpy(&val, var->mem, var->type->bytes);

	ASSERT_FALSEY(err);
	ASSERT_EQUAL(val, 1234);

	free_variable(var);
})

TEST(variable_cannot_write_to_const, {
	Variable *var = make_variable(&TYPE_INT, U"x", true);

	// initial write
	SkullInt data = 1234;
	variable_write(var, &data);

	data = -1234;
	char32_t *err = variable_write(var, &data);

	SkullInt val = 0;
	memcpy(&val, var->mem, var->type->bytes);

	ASSERT_EQUAL(val, 1234);
	ASSERT_TRUTHY(c32scmp(
		ERR_CANNOT_ASSIGN_CONST_(U"x"),
		err
	));

	free(err);
	free_variable(var);
})

TEST(variable_read, {
	Variable *var = make_variable(&TYPE_INT, U"x", false);
	const SkullInt data = 1234;
	variable_write(var, &data);

	SkullInt val = 0;
	variable_read(&val, var);

	ASSERT_EQUAL(val, 1234);

	free_variable(var);
})

TEST(make_variable_with_invalid_name_fails, {
	Variable *var = make_variable(&TYPE_INT, U"1nvalid", false);

	ASSERT_FALSEY(var);

	free_variable(var);
})

TEST(free_variable, {
	free_variable(make_variable(&TYPE_INT, U"x", true));
})

TEST(free_null_variable_is_ok, {
	free_variable(NULL);
})

TEST(fmt_var_int, {
	TEST_FMT_VAR(&TYPE_INT, SkullInt, 1234, "1234");
})

TEST(fmt_var_float, {
	const SkullFloat PI = 3.1415;
	TEST_FMT_VAR(&TYPE_FLOAT, SkullFloat, PI, "3.1415");
})

TEST(fmt_var_float_zero, {
	TEST_FMT_VAR(&TYPE_FLOAT, SkullFloat, 0.0, "0.0");
})

TEST(fmt_var_float_small, {
	const SkullFloat SMALL = 0.0000001;
	TEST_FMT_VAR(&TYPE_FLOAT, SkullFloat, SMALL, "1e-07");
})

TEST(fmt_var_float_trailing_zero, {
	TEST_FMT_VAR(&TYPE_FLOAT, SkullFloat, 1234, "1234.0");
})

TEST(fmt_var_float_infinity, {
	TEST_FMT_VAR(&TYPE_FLOAT, SkullFloat, 1.0 / 0.0, "Infinity");
})

TEST(fmt_var_float_neg_infinity, {
	TEST_FMT_VAR(&TYPE_FLOAT, SkullFloat, 1.0 / -0.0, "-Infinity");
})

TEST(fmt_var_bool, {
	TEST_FMT_VAR(&TYPE_BOOL, bool, false, "false");
})

TEST(fmt_var_rune, {
	TEST_FMT_VAR(&TYPE_RUNE, SkullRune, 'a', "a");
})

TEST(fmt_var_wide_rune_preserved, {
	TEST_FMT_VAR(&TYPE_RUNE, SkullRune, U'存', "存");
})

TEST(fmt_var_str, {
	Variable *var = make_variable(&TYPE_STR, U"x", false);

	char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"\"abc\"", &error);

	eval_assign(var, node, NULL);

	char *str = fmt_var(var);

	ASSERT_TRUTHY(str);
	ASSERT_FALSEY(error);
	ASSERT_EQUAL(strcmp("abc", str), 0);

	free_variable(var);
	free_ast_tree(node);
	free(str);
})

TEST(fmt_var_str_with_escapes, {
	Variable *var = make_variable(&TYPE_STR, U"x", false);

	char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"\" \\r \\n \\t \\\\ \"", &error);

	eval_assign(var, node, NULL);

	char *str = fmt_var(var);

	ASSERT_TRUTHY(str);
	ASSERT_EQUAL(strcmp(" \r \n \t \\ ", str), 0);

	free_variable(var);
	free_ast_tree(node);
	free(str);
})

TEST(fmt_var_str_with_bad_escape, {
	Variable *var = make_variable(&TYPE_STR, U"x", false);

	char32_t *ast_err = NULL;
	AstNode *node = make_ast_tree(U"\"\\z\"", &ast_err);

	char32_t *err = eval_assign(var, node, NULL);

	ASSERT_TRUTHY(err);
	ASSERT_FALSEY(ast_err);
	ASSERT_TRUTHY(c32scmp(ERR_BAD_ESCAPE_(U"\\z"), err));

	free(err);
	free_variable(var);
	free_ast_tree(node);
})

TEST_SELF(variable,
	test_create_variable,
	test_create_variable_with_invalid_type_fails,
	test_variable_write,
	test_variable_cannot_write_to_const,
	test_variable_read,
	test_make_variable_with_invalid_name_fails,
	test_free_variable,
	test_free_null_variable_is_ok,
	test_fmt_var_int,
	test_fmt_var_float,
	test_fmt_var_float_zero,
	test_fmt_var_float_small,
	test_fmt_var_float_trailing_zero,
	test_fmt_var_float_infinity,
	test_fmt_var_float_neg_infinity,
	test_fmt_var_bool,
	test_fmt_var_rune,
	test_fmt_var_wide_rune_preserved,
	test_fmt_var_str,
	test_fmt_var_str_with_escapes,
	test_fmt_var_str_with_bad_escape
)
