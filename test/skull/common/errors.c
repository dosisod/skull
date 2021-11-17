#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/common/errors.c"
#include "skull/semantic/variable.h"

#include "test/skull/common/errors.h"
#include "test/testing.h"

static bool message_stringify_wrapper(Message *error, const char *expected) {
	message_stringify(error);

	if (!error->real) {
		return !expected;
	}
	const bool pass = strcmp(error->real, expected) == 0;

	free(error->real);
	return pass;
}

bool test_fmt_error_stringify(void) {
	Token *tok = tokenize(U"xxx");
	Message err_tok = {
		.tok = tok
	};

	Variable *var = make_variable(TYPE_INT, U"var_name", true);
	Message err_var = {
		.var = var
	};

	Message err_type = {
		.type = TYPE_INT
	};

	const char32_t *str = U"some string";
	Message err_str = {
		.str32 = str
	};

	ASSERT_TRUTHY(message_stringify_wrapper(&err_tok, "xxx"));
	ASSERT_TRUTHY(message_stringify_wrapper(&err_var, "var_name"));
	ASSERT_TRUTHY(message_stringify_wrapper(&err_str, "some string"));
	ASSERT_TRUTHY(message_stringify_wrapper(&err_type, "Int"));

	free_tokens(tok);
	free_variable(var);

	PASS
}

void error_test_self(bool *pass) {
	RUN_ALL(
		test_fmt_error_stringify
	)
}
