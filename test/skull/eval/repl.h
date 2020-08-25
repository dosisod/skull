#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/context.h"
#include "skull/eval/repl.h"

#include "test/testing.h"

TEST(repl_variable_declare, {
	const char32_t *output=repl_eval(U"x: int = 0", NULL);

	return !output;
})

TEST(repl_variable_declare_in_context, {
	context_t *ctx=make_context();

	const char32_t *output=repl_eval(U"x: int = 1234", ctx);

	if (ctx->vars_used!=1) {
		free_context(ctx);
		return false;
	}

	int64_t ret=0;
	variable_read(&ret, ctx->vars[0]);

	const bool pass=(
		!output &&
		ret==1234
	);

	free_context(ctx);
	return pass;
})

TEST(repl_cannot_redeclare_var, {
	context_t *ctx=make_context();

	repl_eval(U"x: int = 1234", ctx);
	const char32_t *output=repl_eval(U"x: int = 1234", ctx);

	const bool pass=(
		ctx->vars_used==1 &&
		c32scmp(
			_ERR_VAR_ALREADY_DEFINED(U"x"),
			output
		)
	);

	free_context(ctx);
	return pass;
})

TEST(repl_declaring_mutable_var, {
	context_t *ctx=make_context();

	repl_eval(U"mut x: int = 1234", ctx);

	const bool pass=(
		ctx->vars_used==1
	);

	free_context(ctx);
	return pass;
})

TEST(repl_declaring_mutable_auto_var, {
	context_t *ctx=make_context();

	repl_eval(U"x := 0", ctx);

	const bool pass=(
		ctx->vars_used==1
	);

	free_context(ctx);
	return pass;
})

TEST(repl_manually_writing_to_const_var_fails, {
	context_t *ctx=make_context();

	repl_eval(U"x: int = 1234", ctx);

	if (ctx->vars_used!=1) {
		free_context(ctx);
		return false;
	}

	int64_t data=1111;
	const char32_t *err=variable_write(ctx->vars[0], &data);

	const bool pass=c32scmp(
		_ERR_CANNOT_ASSIGN_CONST(U"x"),
		err
	);

	free_context(ctx);
	return pass;
})

TEST(repl_manually_writing_to_mutable_var_works, {
	context_t *ctx=make_context();

	repl_eval(U"mut x: int = 1234", ctx);

	if (ctx->vars_used!=1) {
		free_context(ctx);
		return false;
	}

	int64_t data=1111;
	const char32_t *err=variable_write(ctx->vars[0], &data);

	int64_t result=0;
	variable_read(&result, ctx->vars[0]);

	const bool pass=(
		result==1111 &&
		!err
	);

	free_context(ctx);
	return pass;
})

#define TEST_WRITE_TO_MUTABLE_BASE(str_type, str_val1, str_val2, real_type, expected, cmp) \
	context_t *ctx=make_context(); \
	repl_eval(U"mut x: " str_type  " = " str_val1, ctx); \
	const char32_t *output=repl_eval(U"x = " str_val2, ctx); \
	if (ctx->vars_used!=1) { \
		free_context(ctx); \
		return false; \
	} \
	real_type result=0; \
	variable_read(&result, ctx->vars[0]); \
	const bool pass=(!output && (cmp)); \
	free_context(ctx); \
	return pass;

#define TEST_WRITE_TO_MUTABLE_FLOAT(str_type, str_val1, str_val2, real_type, expected) \
	TEST_WRITE_TO_MUTABLE_BASE(str_type, str_val1, str_val2, real_type, expected, (int)result==(int)(expected))

#define TEST_WRITE_TO_MUTABLE(str_type, str_val1, str_val2, real_type, expected) \
	TEST_WRITE_TO_MUTABLE_BASE(str_type, str_val1, str_val2, real_type, expected, result==(expected))

TEST(repl_write_to_mutable_int_var, {
	TEST_WRITE_TO_MUTABLE(
		U"int", U"1234",
		U"5678",
		int64_t, 5678
	);
})

TEST(repl_write_to_mutable_float_var, {
	TEST_WRITE_TO_MUTABLE_FLOAT(
		U"float", U"0.0",
		U"1234.0",
		double, 1234.0
	);
})

TEST(repl_write_to_mutable_bool_var, {
	TEST_WRITE_TO_MUTABLE(
		U"bool", "false",
		U"true",
		bool, true
	);
})

#undef TEST_WRITE_TO_MUTABLE

TEST(repl_write_to_const_var_fails, {
	context_t *ctx=make_context();

	repl_eval(U"x: int = 1234", ctx);
	const char32_t *output=repl_eval(U"x = 5678", ctx);

	if (ctx->vars_used!=1) {
		free_context(ctx);
		return false;
	}

	const bool pass=c32scmp(
		_ERR_CANNOT_ASSIGN_CONST(U"x"),
		output
	);

	free_context(ctx);
	return pass;
})

#define TEST_MAKE_VAR_BASE(str_type, str_val, real_type, expected, cmp) \
	context_t *ctx=make_context(); \
	const char32_t *output=repl_eval(U"x: " str_type " = " str_val, ctx); \
	if (ctx->vars_used!=1) { \
		free_context(ctx); \
		return false; \
	} \
	real_type ret=0; \
	variable_read(&ret, ctx->vars[0]); \
	const bool pass=( \
		!output && \
		(cmp) \
	);\
	free_context(ctx); \
	return pass;

#define TEST_MAKE_VAR_FLOAT(str_type, str_val, real_type, expected) \
	TEST_MAKE_VAR_BASE(str_type, str_val, real_type, expected, (int)ret==(int)(expected))

#define TEST_MAKE_VAR(str_type, str_val, real_type, expected) \
	TEST_MAKE_VAR_BASE(str_type, str_val, real_type, expected, ret==(expected))

TEST(repl_make_float_variable, {
	TEST_MAKE_VAR_FLOAT(
		U"float", U"1234.0",
		double, 1234.0
	);
})

TEST(repl_make_bool_variable, {
	TEST_MAKE_VAR(
		U"bool", U"false",
		bool, false
	);
})

TEST(repl_make_char_variable, {
	TEST_MAKE_VAR(
		U"char", U"'a'",
		char32_t, U'a'
	);
})


TEST(repl_print_fail_with_trailing_tokens, {
	context_t *ctx=make_context();

	repl_eval(U"x: int = 1234", ctx);

	const bool pass=c32scmp(
		ERR_INVALID_INPUT,
		repl_eval(U"x random_data", ctx)
	);

	free_context(ctx);
	return pass;
})

TEST(repl_blank_line_returns_nothing, {
	return !repl_eval(U"", NULL);
})

TEST(repl_invalid_input_returns_error, {
	return c32scmp(
		ERR_INVALID_INPUT,
		repl_eval(U"not_valid", NULL)
	);
})

TEST(repl_mut_cannot_be_used_alone, {
	return c32scmp(
		ERR_INVALID_INPUT,
		repl_eval(U"mut", NULL)
	);
})

TEST(repl_cannot_name_auto_var_after_func, {
	context_t *ctx=make_context();

	const bool pass=c32scmp(
		_ERR_ASSIGN_FUNC(U"print"),
		repl_eval(U"print := 0", ctx)
	);

	free_context(ctx);

	return pass;
})

TEST(repl_clear_function, {
	return c32scmp(
		U"\033[2J\033[;1H",
		repl_eval(U"clear[]", NULL)
	);
})

TEST(repl_print_var, {
	context_t *ctx=make_context();

	repl_eval(U"x: int = 1234", ctx);
	const char32_t *output=repl_eval(U"print[x]", ctx);

	const bool pass=c32scmp(U"1234", output);

	if (!is_error_msg(output)) {
		free((void*)output);
	}
	free_context(ctx);
	return pass;
})

#define TEMP_INT U"99999999999999999999999"
TEST(repl_overflow_int_gives_error, {
	context_t *ctx=make_context();

	const char32_t *output1=repl_eval(U"x: int = " TEMP_INT, ctx);

	repl_eval(U"mut y: int = 0", ctx);
	const char32_t *output2=repl_eval(U"y = " TEMP_INT, ctx);

	const bool pass=(
		c32scmp(_ERR_OVERFLOW(TEMP_INT), output1) &&
		c32scmp(_ERR_OVERFLOW(TEMP_INT), output2)
	);

	free_context(ctx);
	return pass;
})
#undef TEMP_INT

TEST(repl_define_var_without_colon_fails, {
	context_t *ctx=make_context();

	const bool pass=c32scmp(
		ERR_INVALID_INPUT,
		repl_eval(U"x int = 0", ctx)
	);

	free_context(ctx);
	return pass;
})

TEST(repl_missing_value_no_segfault, {
	context_t *ctx=make_context();

	const bool pass=c32scmp(
		_ERR_MISSING_ASSIGNMENT(U"x"),
		repl_eval(U"x: int =", ctx)
	);

	free_context(ctx);
	return pass;
})

TEST(repl_assigning_variable_to_auto_type, {
	context_t *ctx=make_context();

	repl_eval(U"x: int = 1234", ctx);
	const char32_t *output=repl_eval(U"y := x", ctx);

	variable_t *var=context_find_name(ctx, U"y");
	int64_t data=0;
	variable_read(&data, var);

	const bool pass=(
		data==1234 &&
		!output &&
		var->type==&TYPE_INT
	);

	free_context(ctx);
	return pass;
})

TEST(repl_auto_assign_detect_unknown_var, {
	context_t *ctx=make_context();

	const char32_t *output=repl_eval(U"x := y", ctx);

	const bool pass=c32scmp(
		_ERR_VAR_NOT_FOUND(U"y"),
		output
	);

	free_context(ctx);
	return pass;
})

TEST(repl_auto_assign_detect_missing_token, {
	context_t *ctx=make_context();

	const char32_t *output=repl_eval(U"x :=", ctx);

	const bool pass=c32scmp(
		_ERR_MISSING_ASSIGNMENT(U"x"),
		output
	);

	free_context(ctx);
	return pass;
})

TEST(repl_assign_detect_unknown_var, {
	context_t *ctx=make_context();

	const char32_t *output=repl_eval(U"x =", ctx);

	const bool pass=c32scmp(
		_ERR_VAR_NOT_FOUND(U"x"),
		output
	);

	free_context(ctx);
	return pass;
})

TEST(repl_assign_missing_rhs_token, {
	context_t *ctx=make_context();

	repl_eval(U"mut x := 0", ctx);
	const char32_t *output=repl_eval(U"x =", ctx);

	const bool pass=(output==ERR_INVALID_INPUT);

	free_context(ctx);
	return pass;
})

TEST(repl_cannot_reassign_const, {
	context_t *ctx=make_context();

	repl_eval(U"x := \"anything\"", ctx);

	variable_t *var=context_find_name(ctx, U"x");
	if (!var) {
		free_context(ctx);
		return false;
	}

	char32_t *before=NULL;
	variable_read(&before, var);
	before=c32sdup(before);

	const char32_t *output=repl_eval(U"x = \"something else\"", ctx);

	char32_t *after=NULL;
	variable_read(&after, var);

	const bool pass=(
		c32scmp(before, after) &&
		c32scmp(
			_ERR_CANNOT_ASSIGN_CONST(U"x"),
			output
		)
	);

	if (!is_error_msg(output)) {
		free((char32_t*)output);
	}

	free_context(ctx);
	return pass;
})

TEST(repl_cannot_return_non_existent_var, {
	context_t *ctx=make_context();

	const char32_t *output=repl_eval(U"return x", ctx);

	const bool pass=c32scmp(
		_ERR_VAR_NOT_FOUND(U"x"),
		output
	);

	free_context(ctx);
	return pass;
})

TEST(repl_cannot_return_non_int, {
	context_t *ctx=make_context();

	repl_eval(U"x := 'a'", ctx);
	const char32_t *output=repl_eval(U"return x", ctx);

	const bool pass=c32scmp(
		_ERR_NON_INT_RETURN(U"x"),
		output
	);

	free_context(ctx);
	return pass;
})

void repl_test_self(bool *pass) {
	tests_t tests={
		test_repl_variable_declare,
		test_repl_variable_declare_in_context,
		test_repl_cannot_redeclare_var,
		test_repl_declaring_mutable_var,
		test_repl_declaring_mutable_auto_var,
		test_repl_manually_writing_to_const_var_fails,
		test_repl_manually_writing_to_mutable_var_works,
		test_repl_write_to_mutable_int_var,
		test_repl_write_to_mutable_float_var,
		test_repl_write_to_mutable_bool_var,
		test_repl_write_to_const_var_fails,
		test_repl_make_float_variable,
		test_repl_make_bool_variable,
		test_repl_make_char_variable,
		test_repl_print_fail_with_trailing_tokens,
		test_repl_blank_line_returns_nothing,
		test_repl_invalid_input_returns_error,
		test_repl_mut_cannot_be_used_alone,
		test_repl_cannot_name_auto_var_after_func,
		test_repl_clear_function,
		test_repl_print_var,
		test_repl_overflow_int_gives_error,
		test_repl_define_var_without_colon_fails,
		test_repl_missing_value_no_segfault,
		test_repl_assigning_variable_to_auto_type,
		test_repl_auto_assign_detect_unknown_var,
		test_repl_auto_assign_detect_missing_token,
		test_repl_assign_detect_unknown_var,
		test_repl_assign_missing_rhs_token,
		test_repl_cannot_reassign_const,
		test_repl_cannot_return_non_existent_var,
		test_repl_cannot_return_non_int,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
