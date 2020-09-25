#include <stdbool.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/eval_assign.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/variable.h"
#include "skull/parse/classify.h"

#include "test/skull/eval/variable.h"
#include "test/testing.h"

TEST(create_variable, {
	Variable *var = make_variable("int", U"x", true);

	ASSERT_EQUAL(strcmp(var->type->name, "int"), 0);
	ASSERT_TRUTHY(c32scmp(var->name, U"x"));
	ASSERT_TRUTHY(var->is_const);
	ASSERT_EQUAL(var->type->bytes, 8);
	ASSERT_TRUTHY(var->mem);
	ASSERT_FALSEY(var->alloca);

	free_variable(var);
})

TEST(create_variable_with_invalid_type_fails, {
	ASSERT_FALSEY(make_variable("not_a_type", U"x", true));
})

TEST(variable_write, {
	Variable *var = make_variable("int", U"x", false);

	const SkullInt data = 1234;
	const char32_t *ret = variable_write(var, &data);

	SkullInt val = 0;
	memcpy(&val, var->mem, var->type->bytes);

	ASSERT_FALSEY(ret);
	ASSERT_EQUAL(val, 1234);

	free_variable(var);
})

TEST(variable_cannot_write_to_const, {
	Variable *var = make_variable("int", U"x", true);

	const SkullInt data = 1234;
	const char32_t *ret = variable_write(var, &data);

	SkullInt val = 0;
	memcpy(&val, var->mem, var->type->bytes);

	ASSERT_EQUAL(val, 0);
	ASSERT_TRUTHY(c32scmp(
		ERR_CANNOT_ASSIGN_CONST_(U"x"),
		ret
	));

	free_variable(var);
})

TEST(variable_read, {
	Variable *var = make_variable("int", U"x", false);
	const SkullInt data = 1234;
	variable_write(var, &data);

	SkullInt val = 0;
	variable_read(&val, var);

	ASSERT_EQUAL(val, 1234);

	free_variable(var);
})

TEST(make_variable_with_invalid_name_fails, {
	Variable *var = make_variable("int", U"1nvalid", false);

	ASSERT_FALSEY(var);

	free_variable(var);
})

TEST(free_variable, {
	Variable *var = make_variable("int", U"x", true);

	if (!var || !var->mem) {
		free_variable(var);
		FAIL;
	}

	free_variable(var);
})

TEST(free_null_variable_is_ok, {
	free_variable(NULL);
})

TEST(fmt_var_int, {
	TEST_FMT_VAR("int", SkullInt, 1234, U"1234");
})

TEST(fmt_var_float, {
	const SkullFloat PI = 3.1415;
	TEST_FMT_VAR("float", SkullFloat, PI, U"3.1415");
})

TEST(fmt_var_float_zero, {
	TEST_FMT_VAR("float", SkullFloat, 0.0, U"0.0");
})

TEST(fmt_var_float_small, {
	const SkullFloat SMALL = 0.0000001;
	TEST_FMT_VAR("float", SkullFloat, SMALL, U"1e-07");
})

TEST(fmt_var_float_trailing_zero, {
	TEST_FMT_VAR("float", SkullFloat, 1234, U"1234.0");
})

TEST(fmt_var_float_infinity, {
	TEST_FMT_VAR("float", SkullFloat, 1.0 / 0.0, U"Infinity"); // NOLINT
})

TEST(fmt_var_float_neg_infinity, {
	TEST_FMT_VAR("float", SkullFloat, 1.0 / -0.0, U"-Infinity"); // NOLINT
})

TEST(fmt_var_bool, {
	TEST_FMT_VAR("bool", bool, false, U"false");
})

TEST(fmt_var_type, {
	TEST_FMT_VAR("type", Type *, &TYPE_INT, U"int");
})

TEST(fmt_var_rune, {
	TEST_FMT_VAR("rune", SkullRune, 'a', U"a");
})

TEST(fmt_var_wide_rune_preserved, {
	TEST_FMT_VAR("rune", SkullRune, U'存', U"存");
})

#undef TEST_FMT_VAR

TEST(fmt_var_str, {
	Variable *var = make_variable("str", U"x", false);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"\"abc\"", &error);

	eval_assign(var, node, NULL);

	char32_t *str = fmt_var(var);

	ASSERT_TRUTHY(str);
	ASSERT_FALSEY(error);
	ASSERT_TRUTHY(c32scmp(U"abc", str));

	char32_t *mem = NULL;
	variable_read(&mem, var);
	free(mem);

	free(str);
	free_variable(var);
})

TEST(fmt_var_str_with_escapes, {
	Variable *var = make_variable("str", U"x", false);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"\" \\r \\n \\t \\\\ \"", &error);

	eval_assign(var, node, NULL);

	char32_t *str = fmt_var(var);

	ASSERT_TRUTHY(str);
	ASSERT_TRUTHY(c32scmp(U" \r \n \t \\ ", str));

	char32_t *mem = NULL;
	variable_read(&mem, var);
	free(mem);

	free(str);
	free_variable(var);
})

TEST(fmt_var_str_with_bad_escape, {
	Variable *var = make_variable("str", U"x", false);

	const char32_t *ast_err = NULL;
	AstNode *node = make_ast_tree(U"\"\\z\"", &ast_err);

	const char32_t *err = eval_assign(var, node, NULL);

	ASSERT_TRUTHY(err);
	ASSERT_FALSEY(ast_err);
	ASSERT_TRUTHY(c32scmp(ERR_BAD_ESCAPE_(U"\\z"), err));

	char32_t *mem = NULL;
	variable_read(&mem, var);
	free(mem);

	free((char32_t *)err);
	free_variable(var);
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
	test_fmt_var_type,
	test_fmt_var_rune,
	test_fmt_var_wide_rune_preserved,
	test_fmt_var_str,
	test_fmt_var_str_with_escapes,
	test_fmt_var_str_with_bad_escape
)
