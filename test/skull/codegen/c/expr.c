#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/expr.h"
#include "skull/semantic/types.h"
#include "test/testing.h"

bool expr_to_string_fixture(const AstNodeExpr *expr, const char *expected) {
	char *out = expr_node_to_string(expr);

	const bool pass = out && strcmp(out, expected) == 0;

	free(out);
	return pass;
}

bool test_int_expr_to_string() {
	return expr_to_string_fixture(
		&(AstNodeExpr) {
			.type = TYPE_INT,
			.value._int = 1234
		},
		"1234L"
	);
}

bool test_float_expr_to_string() {
	ASSERT_TRUTHY(
		expr_to_string_fixture(
			&(AstNodeExpr) {
				.type = TYPE_FLOAT,
				.value._float = 3.14
			},
			"0x1.91eb851eb851fp+1"
		)
	);

	ASSERT_TRUTHY(
		expr_to_string_fixture(
			&(AstNodeExpr) {
				.type = TYPE_FLOAT,
				.value._float = 0
			},
			"0.0"
		)
	);

	PASS;
}

bool test_bool_expr_to_string() {
	ASSERT_TRUTHY(
		expr_to_string_fixture(
			&(AstNodeExpr) {
				.type = TYPE_BOOL,
				.value._bool = true
			},
			"1"
		)
	);

	ASSERT_TRUTHY(
		expr_to_string_fixture(
			&(AstNodeExpr) {
				.type = TYPE_BOOL,
				.value._bool = false
			},
			"0"
		)
	);

	PASS;
}

bool test_rune_expr_to_string() {
	ASSERT_TRUTHY(
		expr_to_string_fixture(
			&(AstNodeExpr) {
				.type = TYPE_RUNE,
				.value.rune = 'A'
			},
			"0x41"
		)
	);

	PASS;
}

void codegen_c_expr_test_self(bool *pass) {
	RUN_ALL(
		test_int_expr_to_string,
		test_float_expr_to_string,
		test_bool_expr_to_string,
		test_rune_expr_to_string
	)
}
