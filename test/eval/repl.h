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
		wcscmp(L"variable already defined", output)==0
	);

	free_types();
	free_context(ctx);
	return pass;
}

bool test_repl_invalid_input_returns_error(void) {
	const wchar_t *output=repl_eval(L"not_valid", NULL);

	return wcscmp(L"invalid input", output)==0;
}

void repl_test_self(bool *pass) {
	tests_t tests={
		test_repl_variable_assign,
		test_repl_variable_assign_in_context,
		test_repl_cannot_redeclare_var,
		test_repl_invalid_input_returns_error,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
