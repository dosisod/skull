#include <stdbool.h>

#include "../../src/eval/context.h"
#include "../../src/eval/repl.h"
#include "../../test/testing.h"

TEST(repl_variable_declare, {
	make_default_types();
	const wchar_t *output=repl_eval(L"x: int = 0", NULL);

	free_types();
	return output==NULL;
})

TEST(repl_variable_declare_in_context, {
	context_t *ctx=make_context();

	make_default_types();
	const wchar_t *output=repl_eval(L"x: int = 1234", ctx);

	if (ctx->vars_used!=1) {
		free_types();
		free_context(ctx);
		return false;
	}

	int64_t ret=0;
	variable_read(&ret, ctx->vars[0]);

	const bool pass=(
		output==NULL &&
		ret==1234
	);

	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_cannot_redeclare_var, {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"x: int = 1234", ctx);
	const wchar_t *output=repl_eval(L"x: int = 1234", ctx);

	const bool pass=(
		ctx->vars_used==1 &&
		wcscmp(ERROR_MSG[ERROR_VAR_ALREADY_DEFINED], output)==0
	);

	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_declaring_mutable_var, {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"mut x: int = 1234", ctx);

	const bool pass=(
		ctx->vars_used==1
	);

	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_declaring_mutable_auto_var, {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"x := 0", ctx);

	const bool pass=(
		ctx->vars_used==1
	);

	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_manually_writing_to_const_var_fails, {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"x: int = 1234", ctx);

	if (ctx->vars_used!=1) {
		free_context(ctx);
		return false;
	}

	int64_t data=1111;
	const wchar_t *err=variable_write(ctx->vars[0], &data);

	const bool pass=(err==ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST]);

	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_manually_writing_to_mutable_var_works, {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"mut x: int = 1234", ctx);

	if (ctx->vars_used!=1) {
		free_context(ctx);
		return false;
	}

	int64_t data=1111;
	const wchar_t *err=variable_write(ctx->vars[0], &data);

	int64_t result=0;
	variable_read(&result, ctx->vars[0]);

	const bool pass=(
		result==1111 &&
		err==NULL
	);

	free_types();
	free_context(ctx);
	return pass;
})

#define TEST_WRITE_TO_MUTABLE_BASE(str_type, str_val1, str_val2, real_type, expected, cmp) \
	context_t *ctx=make_context(); \
	make_default_types(); \
	repl_eval(L"mut x: " str_type  " = " str_val1, ctx); \
	const wchar_t *output=repl_eval(L"x = " str_val2, ctx); \
	if (ctx->vars_used!=1) { \
		free_context(ctx); \
		return false; \
	} \
	real_type result=0; \
	variable_read(&result, ctx->vars[0]); \
	const bool pass=(output==NULL && (cmp)); \
	free_types(); \
	free_context(ctx); \
	return pass;

#define TEST_WRITE_TO_MUTABLE_FLOAT(str_type, str_val1, str_val2, real_type, expected) \
	TEST_WRITE_TO_MUTABLE_BASE(str_type, str_val1, str_val2, real_type, expected, (int)result==(int)(expected))

#define TEST_WRITE_TO_MUTABLE(str_type, str_val1, str_val2, real_type, expected) \
	TEST_WRITE_TO_MUTABLE_BASE(str_type, str_val1, str_val2, real_type, expected, result==(expected))

TEST(repl_write_to_mutable_int_var, {
	TEST_WRITE_TO_MUTABLE(
		L"int", L"1234",
		L"5678",
		int64_t, 5678
	);
})

TEST(repl_write_to_mutable_float_var, {
	TEST_WRITE_TO_MUTABLE_FLOAT(
		L"float", L"0.0",
		L"1234.0",
		long double, 1234.0
	);
})

TEST(repl_write_to_mutable_bool_var, {
	TEST_WRITE_TO_MUTABLE(
		L"bool", "false",
		L"true",
		bool, true
	);
})

#undef TEST_WRITE_TO_MUTABLE

TEST(repl_write_to_const_var_fails, {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"x: int = 1234", ctx);
	const wchar_t *output=repl_eval(L"x = 5678", ctx);

	if (ctx->vars_used!=1) {
		free_context(ctx);
		return false;
	}

	const bool pass=(wcscmp(ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST], output)==0);

	free_types();
	free_context(ctx);
	return pass;
})

#define TEST_MAKE_VAR_BASE(str_type, str_val, real_type, expected, cmp) \
	context_t *ctx=make_context(); \
	make_default_types(); \
	const wchar_t *output=repl_eval(L"x: " str_type " = " str_val, ctx); \
	if (ctx->vars_used!=1) { \
		free_types(); \
		free_context(ctx); \
		return false; \
	} \
	real_type ret=0; \
	variable_read(&ret, ctx->vars[0]); \
	const bool pass=( \
		output==NULL && \
		(cmp) \
	);\
	free_types(); \
	free_context(ctx); \
	return pass;

#define TEST_MAKE_VAR_FLOAT(str_type, str_val, real_type, expected) \
	TEST_MAKE_VAR_BASE(str_type, str_val, real_type, expected, (int)ret==(int)(expected))

#define TEST_MAKE_VAR(str_type, str_val, real_type, expected) \
	TEST_MAKE_VAR_BASE(str_type, str_val, real_type, expected, ret==(expected))

TEST(repl_make_float_variable, {
	TEST_MAKE_VAR_FLOAT(
		L"float", L"1234.0",
		long double, 1234.0
	);
})

TEST(repl_make_bool_variable, {
	TEST_MAKE_VAR(
		L"bool", L"false",
		bool, false
	);
})

TEST(repl_make_char_variable, {
	TEST_MAKE_VAR(
		L"char", L"'a'",
		wchar_t, L'a'
	);
})


TEST(repl_print_fail_with_trailing_tokens, {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"x: int = 1234", ctx);

	const bool pass=wcscmp(
		ERROR_MSG[ERROR_INVALID_INPUT],
		repl_eval(L"x random_data", ctx)
	)==0;

	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_blank_line_returns_nothing, {
	return repl_eval(L"", NULL)==NULL;
})

TEST(repl_invalid_input_returns_error, {
	return wcscmp(
		ERROR_MSG[ERROR_INVALID_INPUT],
		repl_eval(L"not_valid", NULL)
	)==0;
})

TEST(repl_mut_cannot_be_used_alone, {
	return wcscmp(
		ERROR_MSG[ERROR_INVALID_INPUT],
		repl_eval(L"mut", NULL)
	)==0;
})

TEST(repl_clear_function, {
	return wcscmp(
		L"\033[2J\033[;1H",
		repl_eval(L"clear[]", NULL)
	)==0;
})

TEST(repl_print_var, {
	make_default_types();
	context_t *ctx=make_context();

	repl_eval(L"x: int = 1234", ctx);
	const wchar_t *output=repl_eval(L"x", ctx);

	const bool pass=(wcscmp(L"1234", output)==0);

	if (!is_error_msg(output)) {
		free((void*)output);
	}
	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_add_variables, {
	make_default_types();
	context_t *ctx=make_context();

	repl_eval(L"x: int = 1", ctx);
	repl_eval(L"y: int = 2", ctx); // NOLINT
	const wchar_t *output=repl_eval(L"x + y", ctx); // NOLINT

	const bool pass=(wcscmp(L"3", output)==0);

	if (!is_error_msg(output)) {
		free((void*)output); // NOLINT
	}
	free_types(); // NOLINT
	free_context(ctx);
	return pass;
})

TEST(repl_cannot_add_nonexistent_var, {
	make_default_types();
	context_t *ctx=make_context();

	repl_eval(L"x: int = 1", ctx);
	const wchar_t *output=repl_eval(L"x + y", ctx); // NOLINT

	const bool pass=(wcscmp(ERROR_MSG[ERROR_INVALID_INPUT], output)==0); // NOLINT

	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_overflow_int_gives_error, {
	make_default_types();
	context_t *ctx=make_context();

	const wchar_t *output1=repl_eval(L"x: int = 99999999999999999999999", ctx);

	repl_eval(L"mut y: int = 0", ctx);
	const wchar_t *output2=repl_eval(L"y = 99999999999999999999999", ctx); // NOLINT

	const bool pass=(
		wcscmp(ERROR_MSG[ERROR_OVERFLOW], output1)==0 && // NOLINT
		wcscmp(ERROR_MSG[ERROR_OVERFLOW], output2)==0
	);

	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_define_var_without_colon_fails, {
	make_default_types();
	context_t *ctx=make_context();

	const bool pass=wcscmp(
		ERROR_MSG[ERROR_INVALID_INPUT],
		repl_eval(L"x int = 0", ctx)
	)==0;

	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_missing_value_no_segfault, {
	make_default_types();
	context_t *ctx=make_context();

	const bool pass=wcscmp(
		ERROR_MSG[ERROR_INVALID_INPUT],
		repl_eval(L"x: int =", ctx)
	)==0;

	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_assigning_variable_to_auto_type, {
	make_default_types();
	context_t *ctx=make_context();

	repl_eval(L"x: int = 1234", ctx);
	const wchar_t *output=repl_eval(L"y := x", ctx);

	variable_t *var=context_find_name(ctx, L"y");
	int64_t data=0;
	variable_read(&data, var);

	const bool pass=(
		data==1234 &&
		output==NULL && // NOLINT
		var->type==find_type(L"int")
	);

	free_types();
	free_context(ctx);
	return pass;
})

TEST(repl_auto_assign_detect_unknown_var, {
	context_t *ctx=make_context();

	const wchar_t *output=repl_eval(L"x := oof", ctx);

	const bool pass=(output==ERROR_MSG[ERROR_INVALID_INPUT]);

	free_context(ctx);
	return pass;
})

TEST(repl_auto_assign_detect_bad_token, {
	context_t *ctx=make_context();

	const wchar_t *output=repl_eval(L"x := 1234a", ctx);

	const bool pass=(output==ERROR_MSG[ERROR_INVALID_INPUT]);

	free_context(ctx);
	return pass;
})

TEST(repl_auto_assign_detect_missing_token, {
	context_t *ctx=make_context();

	const wchar_t *output=repl_eval(L"x :=", ctx);

	const bool pass=(output==ERROR_MSG[ERROR_INVALID_INPUT]);

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
		test_repl_clear_function,
		test_repl_add_variables,
		test_repl_print_var,
		test_repl_cannot_add_nonexistent_var,
		test_repl_overflow_int_gives_error,
		test_repl_define_var_without_colon_fails,
		test_repl_missing_value_no_segfault,
		test_repl_assigning_variable_to_auto_type,
		test_repl_auto_assign_detect_unknown_var,
		test_repl_auto_assign_detect_bad_token,
		test_repl_auto_assign_detect_missing_token,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
