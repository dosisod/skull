#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/common/errors.h"
#include "skull/eval/variable.h"

#include "test/skull/common/errors.h"
#include "test/testing.h"

bool fmt_error_stringify_wrapper(ErrorMsg *error, const char *expected) {
	fmt_error_stringify(error);

	if (!error->real) {
		return !expected;
	}
	const bool pass = strcmp(error->real, expected) == 0;

	free(error->real);
	return pass;
}

TEST(fmt_error_stringify, {
	Token *tok = tokenize(U"xxx");
	ErrorMsg err_tok = {
		.tok = tok
	};

	Variable *var = make_variable(&TYPE_INT, U"var_name", true);
	ErrorMsg err_var = {
		.var = var
	};

	ErrorMsg err_type = {
		.type = &TYPE_INT
	};

	const char32_t *str = U"some string";
	ErrorMsg err_str = {
		.str = str
	};

	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_tok, "xxx"));
	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_var, "var_name"));
	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_str, "some string"));
	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_type, "int"));

	free_tokens(tok);
	free_variable(var);
})

TEST_SELF(error,
	test_fmt_error_stringify
)
