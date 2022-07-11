#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/types.h"

#include "test/skull/semantic/types.h"
#include "test/testing.h"

static bool test_find_type(void) {
	SemanticState *state = setup_semantic_state();

	ASSERT_EQUAL(find_type(state, (char[]){"Int"}), &TYPE_INT);

	free_semantic_state(state);

	PASS
}

static bool eval_integer_fixture(const char32_t *str, int64_t expected) {
	Token *token = tokenize(str);

	bool err = false;
	const int64_t i = eval_integer(token, &err);

	ASSERT_EQUAL(i, expected);
	ASSERT_FALSEY(err);

	free_tokens(token);

	PASS;
}

static bool test_eval_int(void) {
	ASSERT_TRUTHY(eval_integer_fixture(U"1234", 1234));
	ASSERT_TRUTHY(eval_integer_fixture(U"-1234", -1234));
	ASSERT_TRUTHY(eval_integer_fixture(U"0b10101", 21));
	ASSERT_TRUTHY(eval_integer_fixture(U"0xFF", 255));
	ASSERT_TRUTHY(eval_integer_fixture(U"0o777", 511));

	ASSERT_TRUTHY(eval_integer_fixture(U"123_456_789", 123456789));
	ASSERT_TRUTHY(eval_integer_fixture(U"0xABC_DEF", 0xABCDEF));
	ASSERT_TRUTHY(eval_integer_fixture(U"0b1010_1100", 172));
	ASSERT_TRUTHY(eval_integer_fixture(U"0o777_777", 262143));

	PASS
}

static double eval_float_fixture(const char32_t *str) {
	Token *token = tokenize(str);

	bool err = false;
	const double f = eval_float(token, &err);

	ASSERT_FALSEY(err);

	free_tokens(token);

	return f;
}

static bool test_eval_float(void) {
	const double inf = eval_float_fixture(U"Infinity");
	ASSERT_TRUTHY(isinf(inf) && inf > 0);

	const double neg_inf = eval_float_fixture(U"Infinity");
	ASSERT_TRUTHY(isinf(neg_inf));

	const double nan = eval_float_fixture(U"NaN");
	ASSERT_TRUTHY(isnan(nan));

	double x = eval_float_fixture(U"1234.0");
	ASSERT_EQUAL((int)x, 1234);

	x = eval_float_fixture(U"-1234.0");
	ASSERT_EQUAL((int)x, -1234);

	x = eval_float_fixture(U"0.0");
	ASSERT_EQUAL((int)x, 0);

	PASS;
}

static bool test_eval_bool(void) {
	Token *token = tokenize(U"true false");

	ASSERT_TRUTHY(eval_bool(token));
	ASSERT_FALSEY(eval_bool(token->next));

	free_tokens(token);

	PASS;
}

static char32_t eval_rune_fixture(const char32_t *str) {
	Token *token = tokenize(str);

	bool err = false;
	const char32_t rune = eval_rune(token, &err);

	ASSERT_FALSEY(err);

	free_tokens(token);

	return rune;
}

static bool test_eval_rune(void) {
	ASSERT_EQUAL(eval_rune_fixture(U"'A'"), 'A');
	ASSERT_EQUAL(eval_rune_fixture(U"'\\0'"), '\0');

	PASS;
}

static bool test_eval_rune_with_control_char_fails(void) {
	Token *token = tokenize(U"\'\n\'");

	bool err = false;
	const char32_t rune = eval_rune(token, &err);

	ASSERT_TRUTHY(err);
	ASSERT_FALSEY(rune);

	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: control character cannot be used in rune\n"
	));

	free_tokens(token);

	PASS
}

static bool test_eval_rune_invalid_escape_fails(void) {
	Token *token = tokenize(U"\'\\x\'");

	bool err = false;
	const char32_t rune = eval_rune(token, &err);

	ASSERT_TRUTHY(err);
	ASSERT_FALSEY(rune);

	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: bad escape sequence: \"\\x\"\n"
	));

	free_tokens(token);

	PASS
}

static bool test_eval_rune_chars_after_escape_invalid(void) {
	Token *token = tokenize(U"\'\\nz\'");

	bool err = false;
	const char32_t rune = eval_rune(token, &err);

	ASSERT_TRUTHY(err);
	ASSERT_FALSEY(rune);

	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: rune contains too many characters\n"
	));

	free_tokens(token);

	PASS
}

static bool test_eval_str_invalid_escape_fails(void) {
	Token *token = tokenize(U"\"\\x\"");

	const char32_t *str = eval_str(token);

	ASSERT_FALSEY(str);

	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: bad escape sequence: \"\\x\"\n"
	));

	free_tokens(token);

	PASS
}

static bool test_eval_str(void) {
	Token *token = tokenize(U"\"abcdef\"");

	char32_t *str = eval_str(token);
	ASSERT_TRUTHY(c32scmp(str, U"abcdef"));

	free(str);
	free_tokens(token);

	PASS;
}

static bool test_add_dynamic_reference_type(void) {
	SemanticState *state = setup_semantic_state();

	Type *type = get_reference_type(state, &TYPE_INT);

	ASSERT_TRUTHY(type);
	ASSERT_EQUAL(strcmp(type->name, "&Int"), 0);

	free_semantic_state(state);

	PASS;
}

void types_test_self(bool *pass) {
	RUN_ALL(
		test_find_type,
		test_eval_int,
		test_eval_float,
		test_eval_bool,
		test_eval_rune,
		test_eval_rune_with_control_char_fails,
		test_eval_rune_invalid_escape_fails,
		test_eval_rune_chars_after_escape_invalid,
		test_eval_str,
		test_eval_str_invalid_escape_fails,
		test_add_dynamic_reference_type
	)
}
