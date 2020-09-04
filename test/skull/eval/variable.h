#include <stdbool.h>
#include <string.h>

#include "skull/errors.h"
#include "skull/eval/eval_assign.h"
#include "skull/eval/variable.h"
#include "skull/parse/classify.h"

#include "test/testing.h"

TEST(create_variable, {
	variable_t *var = make_variable("int", U"x", true);

	const bool pass = (
		strcmp(var->type->name, "int") == 0 &&
		c32scmp(var->name, U"x") &&
		var->is_const &&
		var->bytes == 8 &&
		var->mem
	);

	free_variable(var);

	return pass;
})

TEST(create_variable_with_invalid_type_fails, {
	return !make_variable("not_a_type", U"x", true);
})

TEST(variable_write, {
	variable_t *var = make_variable("int", U"x", false);

	const int64_t data = 1234;
	const char32_t *ret = variable_write(var, &data);

	int64_t val = 0;
	memcpy(&val, var->mem, var->bytes);

	const bool pass=(
		!ret &&
		val == 1234
	);

	free_variable(var);

	return pass;
})

TEST(variable_cannot_write_to_const, {
	variable_t *var = make_variable("int", U"x", true);

	const int64_t data = 1234;
	const char32_t *ret = variable_write(var, &data);

	int64_t val = 0;
	memcpy(&val, var->mem, var->bytes);

	const bool pass=(
		val == 0 &&
		c32scmp(
			ERR_CANNOT_ASSIGN_CONST_(U"x"),
			ret
		)
	);

	free_variable(var);

	return pass;
})

TEST(variable_read, {
	variable_t *var = make_variable("int", U"x", false);
	const int64_t data = 1234;
	variable_write(var, &data);

	int64_t val = 0;
	variable_read(&val, var);

	const bool pass = (val == 1234);

	free_variable(var);

	return pass;
})

TEST(make_variable_with_invalid_name_fails, {
	variable_t *var = make_variable("int", U"1nvalid", false);

	const bool pass = !var;

	free_variable(var);

	return pass;
})

TEST(free_variable, {
	variable_t *var = make_variable("int", U"x", true);

	if (!var || !var->mem) {
		free_variable(var);
		return false;
	}

	free_variable(var);

	return true;
})

TEST(free_null_variable_is_ok, {
	free_variable(NULL);

	return true;
})

#define TEST_FMT_VAR(str_type, real_type, real_data, expected) \
	variable_t *var = make_variable(str_type, U"x", false); \
	real_type data = real_data; \
	variable_write(var, &data); \
	char32_t *str = fmt_var(var); \
	const bool pass = ( \
		str && \
		c32scmp(expected, str) \
	); \
	free(str); \
	free_variable(var); \
	return pass;

TEST(fmt_var_int, {
	TEST_FMT_VAR("int", int64_t, 1234, U"1234");
})

TEST(fmt_var_float, {
	const double PI = 3.1415;
	TEST_FMT_VAR("float", double, PI, U"3.1415");
})

TEST(fmt_var_float_zero, {
	TEST_FMT_VAR("float", double, 0.0, U"0.0");
})

TEST(fmt_var_float_small, {
	const double SMALL = 0.0000001;
	TEST_FMT_VAR("float", double, SMALL, U"1e-07");
})

TEST(fmt_var_float_trailing_zero, {
	TEST_FMT_VAR("float", double, 1234, U"1234.0");
})

TEST(fmt_var_float_infinity, {
	TEST_FMT_VAR("float", double, 1.0 / 0.0, U"Infinity"); // NOLINT
})

TEST(fmt_var_float_neg_infinity, {
	TEST_FMT_VAR("float", double, 1.0 / -0.0, U"-Infinity"); // NOLINT
})

TEST(fmt_var_bool, {
	TEST_FMT_VAR("bool", bool, false, U"false");
})

TEST(fmt_var_type, {
	TEST_FMT_VAR("type", type_t *, &TYPE_INT, U"int");
})

TEST(fmt_var_rune, {
	TEST_FMT_VAR("rune", char32_t, 'a', U"a");
})

TEST(fmt_var_wide_rune_preserved, {
	TEST_FMT_VAR("rune", char32_t, U'存', U"存");
})

#undef TEST_FMT_VAR

TEST(fmt_var_str, {
	variable_t *var = make_variable("str", U"x", false);

	ast_node_t *node = make_ast_tree(U"\"abc\"");

	eval_assign(var, node, NULL);

	char32_t *str = fmt_var(var);

	const bool pass = (
		str &&
		c32scmp(U"abc", str)
	);

	char32_t *mem = NULL;
	variable_read(&mem, var);
	free(mem);

	free(str);
	free_variable(var);
	return pass;
})

TEST(fmt_var_str_with_escapes, {
	variable_t *var = make_variable("str", U"x", false);

	ast_node_t *node = make_ast_tree(U"\"\\r\\n\\t\\\\\"");

	eval_assign(var, node, NULL);

	char32_t *str = fmt_var(var);

	const bool pass = (
		str &&
		c32scmp(U"\r\n\t\\", str)
	);

	char32_t *mem = NULL;
	variable_read(&mem, var);
	free(mem);

	free(str);
	free_variable(var);
	return pass;
})

TEST(fmt_var_str_with_bad_escape, {
	variable_t *var = make_variable("str", U"x", false);

	ast_node_t *node = make_ast_tree(U"\"\\z\"");

	const char32_t *err = eval_assign(var, node, NULL);

	const bool pass = (
		err &&
		c32scmp(ERR_BAD_ESCAPE_(U"\\z"), err)
	);

	char32_t *mem = NULL;
	variable_read(&mem, var);
	free(mem);

	free((char32_t *)err);
	free_variable(var);
	return pass;
})

void variable_test_self(bool *pass) {
	tests_t tests = {
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
		test_fmt_var_str_with_bad_escape,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
