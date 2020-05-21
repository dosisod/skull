#include <stdbool.h>

#include "../../src/eval/context.h"
#include "../../test/testing.h"

bool test_make_context() {
	context_t *ctx=make_context();

	const bool pass=(ctx->vars_used==0);

	free(ctx);

	return pass;
}

bool test_add_vars_to_context() {
	context_t *ctx=make_context();
	variable_t *var=make_variable(L"int32", L"x", true);

	context_add(ctx, var);

	const bool pass=(
		ctx->vars_used==1 &&
		ctx->vars[0]==var
	);

	free(var);
	free(ctx);

	return pass;
}

bool test_cannot_add_same_varname_to_context() {
	context_t *ctx=make_context();
	variable_t *var1=make_variable(L"int32", L"x", true);
	variable_t *var2=make_variable(L"int32", L"x", true);

	context_add(ctx, var1);
	context_add(ctx, var2);

	const bool pass=(
		ctx->vars_used==1 &&
		ctx->vars[0]==var1
	);

	free(var1);
	free(var2);
	free(ctx);

	return pass;
}

bool test_free_context() {
	context_t *ctx=make_context();
	variable_t *var=make_variable(L"int32", L"x", true);

	context_add(ctx, var);

	free_context(ctx);

	return true;
}

void context_test_self(bool *pass) {
	tests_t tests={
		test_make_context,
		test_add_vars_to_context,
		test_cannot_add_same_varname_to_context,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}