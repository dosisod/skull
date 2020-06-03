#include <stdbool.h>

#include "../../src/eval/context.h"
#include "../../src/eval/repl.h"
#include "../../test/testing.h"

bool test_repl_variable_assign(void) {
	make_default_types();
	const wchar_t *output=repl_eval(L"x: int = 0", NULL);

	free_types();
	return output==NULL;
}

bool test_repl_variable_assign_in_context(void) {
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
}

bool test_repl_cannot_redeclare_var(void) {
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
}

bool test_repl_declaring_mutable_var(void) {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"mut x: int = 1234", ctx);

	const bool pass=(
		ctx->vars_used==1
	);

	free_types();
	free_context(ctx);
	return pass;
}

bool test_repl_manually_writing_to_const_var_fails(void) {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"x: int = 1234", ctx);

	if (ctx->vars_used!=1) {
		free_context(ctx);
		return false;
	}

	int64_t data=1111;
	uint8_t err=variable_write(ctx->vars[0], &data);

	const bool pass=(err==VARIABLE_WRITE_ECONST);

	free_types();
	free_context(ctx);
	return pass;
}

bool test_repl_manually_writing_to_mutable_var_works(void) {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"mut x: int = 1234", ctx);

	if (ctx->vars_used!=1) {
		free_context(ctx);
		return false;
	}

	int64_t data=1111;
	uint8_t err=variable_write(ctx->vars[0], &data);

	int64_t result=0;
	variable_read(&result, ctx->vars[0]);

	const bool pass=(
		result==1111 &&
		err==VARIABLE_WRITE_OK
	);

	free_types();
	free_context(ctx);
	return pass;
}

bool test_repl_write_to_mutable_var(void) {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"mut x: int = 1234", ctx);
	repl_eval(L"x = 5678", ctx);

	if (ctx->vars_used!=1) {
		free_context(ctx);
		return false;
	}

	int64_t result=0;
	variable_read(&result, ctx->vars[0]);

	const bool pass=(result==5678);

	free_types();
	free_context(ctx);
	return pass;
}

bool test_repl_write_to_const_var_fails(void) {
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
}

bool test_repl_print_fail_with_trailing_tokens(void) {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"x: int = 1234", ctx);
	const wchar_t *output=repl_eval(L"x random_data", ctx);

	const bool pass=wcscmp(ERROR_MSG[ERROR_INVALID_INPUT], output)==0;

	free_types();
	free_context(ctx);
	return pass;
}

bool test_repl_blank_line_returns_nothing(void) {
	return repl_eval(L"", NULL)==NULL;
}

bool test_repl_invalid_input_returns_error(void) {
	const wchar_t *output=repl_eval(L"not_valid", NULL);

	return wcscmp(ERROR_MSG[ERROR_INVALID_INPUT], output)==0;
}

bool test_repl_mut_cannot_be_used_alone(void) {
	return wcscmp(
		ERROR_MSG[ERROR_INVALID_INPUT],
		repl_eval(L"mut", NULL)
	)==0;
}

bool test_repl_clear_function(void) {
	return wcscmp(
		L"\033[2J\033[;1H",
		repl_eval(L"clear[]", NULL)
	)==0;
}

bool test_repl_add_variables(void) {
	make_default_types();
	context_t *ctx=make_context();

	repl_eval(L"x: int = 1", ctx);
	repl_eval(L"y: int = 2", ctx);
	const wchar_t *output=repl_eval(L"x + y", ctx);

	const bool pass=(output==NULL);

	free_types();
	free_context(ctx);
	return pass;
}

bool test_repl_cannot_add_nonexistent_var(void) {
	make_default_types();
	context_t *ctx=make_context();

	repl_eval(L"x: int = 1", ctx);
	const wchar_t *output=repl_eval(L"x + y", ctx);

	const bool pass=(wcscmp(ERROR_MSG[ERROR_INVALID_INPUT], output)==0);

	free_types();
	free_context(ctx);
	return pass;
}

void repl_test_self(bool *pass) {
	tests_t tests={
		test_repl_variable_assign,
		test_repl_variable_assign_in_context,
		test_repl_cannot_redeclare_var,
		test_repl_declaring_mutable_var,
		test_repl_manually_writing_to_const_var_fails,
		test_repl_manually_writing_to_mutable_var_works,
		test_repl_write_to_mutable_var,
		test_repl_write_to_const_var_fails,
		test_repl_print_fail_with_trailing_tokens,
		test_repl_blank_line_returns_nothing,
		test_repl_invalid_input_returns_error,
		test_repl_mut_cannot_be_used_alone,
		test_repl_clear_function,
		test_repl_add_variables,
		test_repl_cannot_add_nonexistent_var,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
