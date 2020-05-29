#include <stdbool.h>

#include "../../src/eval/context.h"
#include "../../src/eval/repl.h"
#include "../../test/testing.h"

bool test_repl_number_echo() {
	const wchar_t *output=repl_eval(L"0", NULL);

	return wcscmp(L"0", output)==0;
}

bool test_repl_variable_assign() {
	make_default_types();
	const wchar_t *output=repl_eval(L"x: int = 0", NULL);

	free_types();
	return output==NULL;
}

bool test_repl_variable_assign_in_context() {
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

bool test_repl_cannot_redeclare_var() {
	context_t *ctx=make_context();

	make_default_types();
	repl_eval(L"x: int = 1234", ctx);
	repl_eval(L"x: int = 1234", ctx);

	const bool pass=(ctx->vars_used==1);

	free_types();
	free_context(ctx);
	return pass;
}

void repl_test_self(bool *pass) {
	tests_t tests={
		test_repl_number_echo,
		test_repl_variable_assign,
		test_repl_variable_assign_in_context,
		test_repl_cannot_redeclare_var,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
