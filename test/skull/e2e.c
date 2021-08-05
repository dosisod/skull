#include <stdbool.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/codegen/entry.h"
#include "skull/common/local.h"

bool SKULL_TESTING = 0;
#define TEST_DIR "./test/sh"

int e2e_wrapper(const char *code, const char *mock_file, const char *error_file) {
	char *tmp = strdup(code);
	BUILD_DATA.error_file = error_file;
	return init_codegen_pipeline(mock_file, tmp);
}

int main(void) {
	setup_locale();

	bool pass = false;


pass |= e2e_wrapper(
"	mut bool := true\n" \
"	mut int := 1234\n" \
"	mut float := 3.14\n" \
"	mut str := \"hello world\"\n" \
"	mut rune := 'x'\n",
TEST_DIR"/declare/basic_types.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := true and true\n",
TEST_DIR"/declare/bool_expr/and.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1 >= 1\n" \
"	mut y := 1.0 >= 1.0\n",
TEST_DIR"/declare/bool_expr/gtr_than_eq.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 2 > 1\n" \
"	mut y := 2.0 > 1.0\n",
TEST_DIR"/declare/bool_expr/gtr_than.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1 isnt 2\n",
TEST_DIR"/declare/bool_expr/isnt.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1 is 1\n" \
"	mut y := 1.0 is 1.0\n" \
"	mut z := 'x' is 'x'\n" \
"	mut q := true is true\n",
TEST_DIR"/declare/bool_expr/is.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1 <= 1\n" \
"	mut y := 1.0 <= 1.0\n",
TEST_DIR"/declare/bool_expr/less_than_eq.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1 < 2\n" \
"	mut y := 1.0 < 2.0\n",
TEST_DIR"/declare/bool_expr/less_than.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := not false\n",
TEST_DIR"/declare/bool_expr/not.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := false or true\n",
TEST_DIR"/declare/bool_expr/or.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	x := 1\n" \
"	\n" \
"	y := 1 is x\n",
TEST_DIR"/declare/bool_expr/var_rhs.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := false xor true\n",
TEST_DIR"/declare/bool_expr/xor.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	export x := 1\n",
TEST_DIR"/declare/export_var.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 0\n" \
"	mut y := 0\n",
TEST_DIR"/declare/many_vars.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x: Int = 0\n",
TEST_DIR"/declare/mut_with_explicit_type.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := \"abc\"\n" \
"	x = \"defg\"\n",
TEST_DIR"/declare/reassign_str.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 0\n" \
"	\n" \
"	x = 1\n",
TEST_DIR"/declare/reassign_var.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	x := \"# \"\n",
TEST_DIR"/declare/str_with_comment.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 0\n" \
"	mut z := x\n" \
"	\n" \
"	return z\n",
TEST_DIR"/declare/variable_auto_deduce.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	if false {\n" \
		"noop\n" \
"	}\n" \
"	elif true {\n" \
		"noop\n" \
"	}\n" \
"	else {\n" \
		"noop\n" \
"	}\n" \
"	\n" \
"	f() Bool { return false }\n" \
"	\n" \
"	if f() {\n" \
		"noop\n" \
"	}\n" \
"	elif f() {\n" \
		"noop\n" \
"	}\n",
TEST_DIR"/flow/elif.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	if true {\n" \
		"# do nothing\n" \
"	}\n" \
"	else {\n" \
		"# do nothing\n" \
"	}\n",
TEST_DIR"/flow/else.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	if true {\n" \
		"return 1\n" \
"	}\n" \
"	# comment\n" \
"	else {\n" \
		"return 2\n" \
"	}\n",
TEST_DIR"/flow/if/else_comment.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	if false {\n" \
	  "return 0\n" \
"	}\n" \
"	\n" \
"	return 1\n",
TEST_DIR"/flow/if/false.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	if not true {\n" \
		"# do nothing\n" \
"	}\n",
TEST_DIR"/flow/if/not_with_const.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := true\n" \
"	\n" \
"	if not x {\n" \
		"# do nothing\n" \
"	}\n",
TEST_DIR"/flow/if/not_with_var.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	if true {\n" \
	  "return 0\n" \
"	}\n" \
"	\n" \
"	return 1\n",
TEST_DIR"/flow/if/true.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := false\n" \
"	\n" \
"	if x {\n" \
		"return 1\n" \
"	}\n" \
"	\n" \
"	return 0\n",
TEST_DIR"/flow/if/with_var_false.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := true\n" \
"	\n" \
"	if x {\n" \
		"return 1\n" \
"	}\n" \
"	\n" \
"	return 0\n",
TEST_DIR"/flow/if/with_var_true.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	if true {\n" \
		"if true {\n" \
			"return 1\n" \
		"}\n" \
"	}\n" \
"	\n" \
"	return 0\n",
TEST_DIR"/flow/nested_if.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	return 0\n",
TEST_DIR"/flow/return_0.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	return 1 + 2\n",
TEST_DIR"/flow/return_expr.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external f() Int\n" \
"	\n" \
"	return f()\n",
TEST_DIR"/flow/return_func_value.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1\n" \
"	\n" \
"	return x\n",
TEST_DIR"/flow/return_var.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	if true {\n" \
		"mut x := 1\n" \
"	}\n" \
"	\n" \
"	return 0\n",
TEST_DIR"/flow/set_var_in_if.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	return 0\n" \
"	unreachable\n" \
"	# some comment\n" \
"	noop\n",
TEST_DIR"/flow/unreachable_ignored.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	f() {\n" \
		"unreachable\n" \
"	}\n",
TEST_DIR"/flow/unreachable_in_func.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	while true {\n" \
		"# do nothing\n" \
"	}\n" \
"	\n" \
"	i := 0\n" \
"	\n" \
"	while i < 10 {\n" \
		"noop\n" \
"	}\n" \
"	\n" \
"	f() Bool { return true }\n" \
"	\n" \
"	while f() {\n" \
		"noop\n" \
"	}\n",
TEST_DIR"/flow/while.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external func() Int\n" \
"	\n" \
"	mut x := func()\n" \
"	\n" \
"	return x\n",
TEST_DIR"/function/assign_return_value.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external puts(s: Str)\n" \
"	\n" \
"	puts(\"hello world!\")\n",
TEST_DIR"/function/call_str.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	f() {\n" \
		"# do nothing\n" \
"	}\n",
TEST_DIR"/function/declare_func.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	x() { noop }\n",
TEST_DIR"/function/empty_func_no_err.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	export f() {\n" \
		"# do nothing\n" \
"	}\n",
TEST_DIR"/function/exported.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	f(num: Int) {\n" \
		"# do nothing\n" \
"	}\n" \
"	\n" \
"	f(1 + 2)\n",
TEST_DIR"/function/expr_param.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external x() Int\n" \
"	external y(num: Int) Int\n",
TEST_DIR"/function/external_return_type.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external x(a: Int)\n",
TEST_DIR"/function/external_with_param.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	f(x: Int) Int {\n" \
		"return x\n" \
"	}\n" \
"	\n" \
"	f(f(1))\n",
TEST_DIR"/function/func_expr_param.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	f() {\n" \
		"mut x := 1\n" \
"	}\n" \
"	\n" \
"	mut x := 0\n",
TEST_DIR"/function/func_with_scope.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	f(a: Int, b: Float) {\n" \
		"# do nothing\n" \
"	}\n",
TEST_DIR"/function/multi_param_func_different_types.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	f(a: Int, b: Int) Int {\n" \
		"x := a + b\n" \
"	\n" \
		"return x\n" \
"	}\n" \
"	\n" \
"	f(1, 2)\n",
TEST_DIR"/function/multi_param_func.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external a()\n" \
"	external b()\n" \
"	\n" \
"	a()\n",
TEST_DIR"/function/multiple_externals.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	f(num: Int) Int {\n" \
		"return num\n" \
"	}\n" \
"	\n" \
"	f(1)\n",
TEST_DIR"/function/multi_var_func.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	f() {\n" \
		"if true {\n" \
			"# do nothing\n" \
		"}\n" \
"	}\n",
TEST_DIR"/function/nested_if.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external x()\n" \
"	\n" \
"	x()\n",
TEST_DIR"/function/no_args.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external x()\n" \
"	\n" \
"	x()\n" \
"	\n" \
"	return 1\n",
TEST_DIR"/function/nodes_after_call_not_ignored.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	func() Float {\n" \
		"mut ret := 3.14\n" \
		"return ret\n" \
"	}\n" \
"	\n" \
"	func2() Float {\n" \
		"return 3.14\n" \
"	}\n",
TEST_DIR"/function/non_int_return.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external x(a: Int)\n" \
"	\n" \
"	x(1)\n",
TEST_DIR"/function/single_param.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external f(x: Int)\n" \
"	\n" \
"	mut num := 1234\n" \
"	\n" \
"	f(num)\n",
TEST_DIR"/function/var_param.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	f() {\n" \
		"return\n" \
"	}\n",
TEST_DIR"/function/void_explicit_ret.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	# comment\n" \
"	x := 0\n",
TEST_DIR"/misc/comment_then_code.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	x := 1\n" \
"	\n" \
"	y := x + 1\n" \
"	\n" \
"	z := y + x\n" \
"	\n" \
"	return z\n",
TEST_DIR"/misc/const_inline.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := \"\\n\"\n" \
"	mut y := '\\n'\n",
TEST_DIR"/misc/escape_sequences.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut is_this_kept := true\n" \
"	#{\n" \
"	mut and_this := false\n" \
"	#}\n" \
"	mut what_about_this := true\n",
TEST_DIR"/misc/ignore_block_comments.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	# comment will not cause any issues\n" \
"	\n" \
"	return 0\n",
TEST_DIR"/misc/ignore_comments.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	emoji := \"😀\"\n",
TEST_DIR"/misc/locale.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	#{\n" \
"	#{\n" \
"	#}\n",
TEST_DIR"/misc/nested_block_comment.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	return 0",
TEST_DIR"/misc/no_eol_works.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	if true {\n" \
		"noop\n" \
"	}\n" \
"	\n" \
"	noop\n",
TEST_DIR"/misc/noop.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	if false {\n" \
		"x := Int\n" \
"	}\n",
TEST_DIR"/misc/not_top_lvl_alias.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	x := 0\n" \
"	\n" \
"	if true {\n" \
		"if true {\n" \
			"# do nothing\n" \
		"}\n" \
"	\n" \
		"z := x\n" \
"	}\n",
TEST_DIR"/misc/preserve_nested_scopes.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	f(a: Int, b: Int, c: Int) { noop }\n",
TEST_DIR"/misc/triple_param_no_overflow.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	int_alias := Int\n" \
"	\n" \
"	mut x: int_alias = 1234\n" \
"	\n" \
"	f(num: int_alias) int_alias {\n" \
		"return num\n" \
"	}\n",
TEST_DIR"/misc/type_alias.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 2.0 + 3.0\n",
TEST_DIR"/oper/add_float_consts.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1 + 2\n",
TEST_DIR"/oper/add_int_consts.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 4.0 / 2.0\n",
TEST_DIR"/oper/div_float_consts.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 4 / 2\n",
TEST_DIR"/oper/div_int_consts.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external f() Int\n" \
"	\n" \
"	x := f() / f()\n",
TEST_DIR"/oper/div_with_expr.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 2.0 ^ 3.0\n",
TEST_DIR"/oper/float_pow.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external f() Int\n" \
"	\n" \
"	mut x := f() + 1\n",
TEST_DIR"/oper/func_lhs.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	external f() Int\n" \
"	\n" \
"	mut x := 1 + f()\n",
TEST_DIR"/oper/func_rhs.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 10 ^ 3\n",
TEST_DIR"/oper/int_pow.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 3.14 is 3.14\n",
TEST_DIR"/oper/is_cmp_float.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1234 is 1234\n",
TEST_DIR"/oper/is_cmp_int.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 'x' is 'x'\n",
TEST_DIR"/oper/is_cmp_rune.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := \"x\" is \"x\"\n",
TEST_DIR"/oper/is_cmp_str.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1\n" \
"	mut y := x + 2\n" \
"	mut z := x < 1\n",
TEST_DIR"/oper/lhs_with_var.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1 << 2\n",
TEST_DIR"/oper/lshift.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 9 mod 2\n" \
"	mut y := -10 mod 3\n" \
"	mut z := 12.0 mod 5.0\n",
TEST_DIR"/oper/mod.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 2.0 * 3.0\n",
TEST_DIR"/oper/mult_float_consts.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 2 * 3\n",
TEST_DIR"/oper/mult_int_consts.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	x := 1\n" \
"	\n" \
"	return - x\n",
TEST_DIR"/oper/negative_unary.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := (1 + 2)\n" \
"	\n" \
"	mut y := (((1 + 2)))\n",
TEST_DIR"/oper/paren_expr.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1 + (2 + 3)\n",
TEST_DIR"/oper/paren_rhs_expr.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1\n" \
"	\n" \
"	mut z := 2 + x\n",
TEST_DIR"/oper/rhs_with_var.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	return 0b1111 >> 2\n",
TEST_DIR"/oper/rshift.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 2.0 - 3.0\n",
TEST_DIR"/oper/sub_float_consts.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut x := 1 - 2\n",
TEST_DIR"/oper/sub_int_consts.sk",
"/dev/null"
);


pass |= e2e_wrapper(
"	mut w := - 1.0\n" \
"	mut x := not not false\n" \
"	mut y := - 1\n" \
"	mut z := - y\n",
TEST_DIR"/oper/unary_expr.sk",
"/dev/null"
);


// error tests

pass |= !e2e_wrapper(
"x := 0\n" \
"\n" \
"z: Bool = x\n",
TEST_DIR"/error/declare/assign_mismatch_var_type.sk",
TEST_DIR"/error/declare/.assign_mismatch_var_type.sk.out"
);

pass |= !e2e_wrapper(
"x: Int =\n",
TEST_DIR"/error/declare/assign_missing_rhs.sk",
TEST_DIR"/error/declare/.assign_missing_rhs.sk.out"
);



pass |= !e2e_wrapper(
"mut x := \"this will\" / \"fail\"\n",
TEST_DIR"/error/declare/assign_unassignable_expression.sk",
TEST_DIR"/error/declare/.assign_unassignable_expression.sk.out"
);


pass |= !e2e_wrapper(
"x := y\n",
TEST_DIR"/error/declare/assign_unknown_var.sk",
TEST_DIR"/error/declare/.assign_unknown_var.sk.out"
);


pass |= !e2e_wrapper(
"x :=\n",
TEST_DIR"/error/declare/auto_assign_missing_rhs.sk",
TEST_DIR"/error/declare/.auto_assign_missing_rhs.sk.out"
);


pass |= !e2e_wrapper(
"x: fail = 1\n",
TEST_DIR"/error/declare/bad_type_not_allowed.sk",
TEST_DIR"/error/declare/.bad_type_not_allowed.sk.out"
);


pass |= !e2e_wrapper(
"mut x: Int = 1 is 1\n",
TEST_DIR"/error/declare/check_bool_assign.sk",
TEST_DIR"/error/declare/.check_bool_assign.sk.out"
);


pass |= !e2e_wrapper(
"mut x: Int = not true\n",
TEST_DIR"/error/declare/check_bool_not_assign.sk",
TEST_DIR"/error/declare/.check_bool_not_assign.sk.out"
);


pass |= !e2e_wrapper(
"x := '\n" \
"'\n",
TEST_DIR"/error/declare/control_char_in_rune.sk",
TEST_DIR"/error/declare/.control_char_in_rune.sk.out"
);


pass |= !e2e_wrapper(
"x: Float = 1 + 2\n",
TEST_DIR"/error/declare/expr_assign_bad_type.sk",
TEST_DIR"/error/declare/.expr_assign_bad_type.sk.out"
);


pass |= !e2e_wrapper(
"x := 999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0\n",
TEST_DIR"/error/declare/float_overflow.sk",
TEST_DIR"/error/declare/.float_overflow.sk.out"
);


pass |= !e2e_wrapper(
"x := 99999999999999999999999999999999\n",
TEST_DIR"/error/declare/int_overflow.sk",
TEST_DIR"/error/declare/.int_overflow.sk.out"
);


pass |= !e2e_wrapper(
"x := -99999999999999999999999999999999\n",
TEST_DIR"/error/declare/int_underflow.sk",
TEST_DIR"/error/declare/.int_underflow.sk.out"
);


pass |= !e2e_wrapper(
"x := 0\n" \
"x = 1\n",
TEST_DIR"/error/declare/reassign_const.sk",
TEST_DIR"/error/declare/.reassign_const.sk.out"
);


pass |= !e2e_wrapper(
"mut x := 0\n" \
"x =\n",
TEST_DIR"/error/declare/reassign_missing_rhs.sk",
TEST_DIR"/error/declare/.reassign_missing_rhs.sk.out"
);


pass |= !e2e_wrapper(
"x = 0\n",
TEST_DIR"/error/declare/reassign_non_existet_var.sk",
TEST_DIR"/error/declare/.reassign_non_existet_var.sk.out"
);


pass |= !e2e_wrapper(
"external x()\n" \
"external x()\n",
TEST_DIR"/error/declare/redeclare_external.sk",
TEST_DIR"/error/declare/.redeclare_external.sk.out"
);


pass |= !e2e_wrapper(
"x := 0\n" \
"x := 0\n",
TEST_DIR"/error/declare/var_already_defined.sk",
TEST_DIR"/error/declare/.var_already_defined.sk.out"
);


pass |= !e2e_wrapper(
"x := 1\n" \
"x := Int\n",
TEST_DIR"/error/declare/var_as_alias.sk",
TEST_DIR"/error/declare/.var_as_alias.sk.out"
);


pass |= !e2e_wrapper(
"# this wont work\n" \
"else {\n" \
"	noop\n" \
"}\n",
TEST_DIR"/error/flow/comment_then_else.sk",
TEST_DIR"/error/flow/.comment_then_else.sk.out"
);


pass |= !e2e_wrapper(
"else {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/flow/else_without_if.sk",
TEST_DIR"/error/flow/.else_without_if.sk.out"
);


pass |= !e2e_wrapper(
"if true {}\n",
TEST_DIR"/error/flow/empty_block.sk",
TEST_DIR"/error/flow/.empty_block.sk.out"
);


pass |= !e2e_wrapper(
"if true\n",
TEST_DIR"/error/flow/if_missing_block.sk",
TEST_DIR"/error/flow/.if_missing_block.sk.out"
);


pass |= !e2e_wrapper(
"if true {\n" \
"	mut x := 0\n" \
"}\n" \
"\n" \
"return x\n",
TEST_DIR"/error/flow/if_var_out_of_scope.sk",
TEST_DIR"/error/flow/.if_var_out_of_scope.sk.out"
);


pass |= !e2e_wrapper(
"if x {\n" \
"	return 1\n" \
"}\n" \
"\n" \
"return 0\n",
TEST_DIR"/error/flow/if_with_nonexisting_var.sk",
TEST_DIR"/error/flow/.if_with_nonexisting_var.sk.out"
);


pass |= !e2e_wrapper(
"if true {\n" \
"	x 1\n" \
"}\n" \
"\n" \
"return 2\n",
TEST_DIR"/error/flow/invalid_return_in_if.sk",
TEST_DIR"/error/flow/.invalid_return_in_if.sk.out"
);


pass |= !e2e_wrapper(
"if # this will fail\n",
TEST_DIR"/error/flow/missing_expr.sk",
TEST_DIR"/error/flow/.missing_expr.sk.out"
);


pass |= !e2e_wrapper(
"return 1.0\n",
TEST_DIR"/error/flow/non_int_return.sk",
TEST_DIR"/error/flow/.non_int_return.sk.out"
);


pass |= !e2e_wrapper(
"return 0xffffffffffffffff\n",
TEST_DIR"/error/flow/return_int_overflow.sk",
TEST_DIR"/error/flow/.return_int_overflow.sk.out"
);


pass |= !e2e_wrapper(
"return # wont work\n",
TEST_DIR"/error/flow/return_missing_expr.sk",
TEST_DIR"/error/flow/.return_missing_expr.sk.out"
);


pass |= !e2e_wrapper(
"return x\n",
TEST_DIR"/error/flow/return_non_existent_var.sk",
TEST_DIR"/error/flow/.return_non_existent_var.sk.out"
);


pass |= !e2e_wrapper(
"if true {\n" \
"	noop\n" \
"}\n" \
"x := 1\n" \
"elif true {\n" \
"	noop\n" \
"}\n",
TEST_DIR"/error/flow/stmt_between_elif.sk",
TEST_DIR"/error/flow/.stmt_between_elif.sk.out"
);


pass |= !e2e_wrapper(
"f() {\n" \
"	return\n" \
"	x := 1\n" \
"}\n",
TEST_DIR"/error/flow/unreachable_in_func.sk",
TEST_DIR"/error/flow/.unreachable_in_func.sk.out"
);


pass |= !e2e_wrapper(
"return 1\n" \
"\n" \
"x := 0\n",
TEST_DIR"/error/flow/unreachable_return.sk",
TEST_DIR"/error/flow/.unreachable_return.sk.out"
);


pass |= !e2e_wrapper(
"x := Int\n" \
"x() { noop }\n",
TEST_DIR"/error/func/alias_as_func.sk",
TEST_DIR"/error/func/.alias_as_func.sk.out"
);


pass |= !e2e_wrapper(
"# TODO: fix incorrect error (should be cannot assign type)\n" \
"\n" \
"func() Int {\n" \
"	return 1\n" \
"}\n" \
"\n" \
"x: Float = func()\n",
TEST_DIR"/error/func/assign_bad_type.sk",
TEST_DIR"/error/func/.assign_bad_type.sk.out"
);


pass |= !e2e_wrapper(
"f() {\n" \
"	# do nothing\n" \
"}\n" \
"\n" \
"x := f()\n",
TEST_DIR"/error/func/assign_void_ret.sk",
TEST_DIR"/error/func/.assign_void_ret.sk.out"
);


pass |= !e2e_wrapper(
"f(b: Bool) {\n" \
"	# do nothing\n" \
"}\n" \
"\n" \
"f(1 + 2)\n",
TEST_DIR"/error/func/bad_expr_param.sk",
TEST_DIR"/error/func/.bad_expr_param.sk.out"
);


pass |= !e2e_wrapper(
"external x(num: Int)\n" \
"\n" \
"x(false)\n",
TEST_DIR"/error/func/bad_param_type.sk",
TEST_DIR"/error/func/.bad_param_type.sk.out"
);


pass |= !e2e_wrapper(
"x() { noop }\n" \
"x := Int\n",
TEST_DIR"/error/func/func_as_alias.sk",
TEST_DIR"/error/func/.func_as_alias.sk.out"
);


pass |= !e2e_wrapper(
"x() { return }\n" \
"\n" \
"x := 1\n",
TEST_DIR"/error/func/func_as_var.sk",
TEST_DIR"/error/func/.func_as_var.sk.out"
);


pass |= !e2e_wrapper(
"f(x: Int) {\n" \
"	# do nothing\n" \
"}\n" \
"\n" \
"f(1, 2, 3)\n",
TEST_DIR"/error/func/invalid_num_params.sk",
TEST_DIR"/error/func/.invalid_num_params.sk.out"
);


pass |= !e2e_wrapper(
"return f(",
TEST_DIR"/error/func/missing_closing_paren.sk",
TEST_DIR"/error/func/.missing_closing_paren.sk.out"
);


pass |= !e2e_wrapper(
"x := f()\n",
TEST_DIR"/error/func/missing_declaration.sk",
TEST_DIR"/error/func/.missing_declaration.sk.out"
);


pass |= !e2e_wrapper(
"f() Int {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/func/missing_return.sk",
TEST_DIR"/error/func/.missing_return.sk.out"
);


pass |= !e2e_wrapper(
"external f(a: Int, b: Int)\n" \
"\n" \
"f(1 2)\n",
TEST_DIR"/error/func/multi_param_call_missing_comma.sk",
TEST_DIR"/error/func/.multi_param_call_missing_comma.sk.out"
);


pass |= !e2e_wrapper(
"f(x: Int y: Int) {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/func/multi_param_missing_comma.sk",
TEST_DIR"/error/func/.multi_param_missing_comma.sk.out"
);


pass |= !e2e_wrapper(
"external f(x: Int)\n" \
"\n" \
"x := false\n" \
"\n" \
"f(x)\n",
TEST_DIR"/error/func/param_bad_type.sk",
TEST_DIR"/error/func/.param_bad_type.sk.out"
);


pass |= !e2e_wrapper(
"external f(x: Int)\n" \
"\n" \
"f(fail)\n",
TEST_DIR"/error/func/param_unknown_var.sk",
TEST_DIR"/error/func/.param_unknown_var.sk.out"
);


pass |= !e2e_wrapper(
"f() {\n" \
"	# do nothing\n" \
"}\n" \
"\n" \
"f(x)\n",
TEST_DIR"/error/func/passing_to_paramless_func.sk",
TEST_DIR"/error/func/.passing_to_paramless_func.sk.out"
);


pass |= !e2e_wrapper(
"f(x: does_not_exist) {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/func/proto_param_bad_type.sk",
TEST_DIR"/error/func/.proto_param_bad_type.sk.out"
);


pass |= !e2e_wrapper(
"f() does_not_exist {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/func/proto_return_bad_type.sk",
TEST_DIR"/error/func/.proto_return_bad_type.sk.out"
);


pass |= !e2e_wrapper(
"f() Int {\n" \
"	return false\n" \
"}\n",
TEST_DIR"/error/func/return_type_mismatch.sk",
TEST_DIR"/error/func/.return_type_mismatch.sk.out"
);


pass |= !e2e_wrapper(
"x := 0\n" \
"\n" \
"f(x: Int) Int {\n" \
"	return x\n" \
"}\n",
TEST_DIR"/error/func/shadowed_param.sk",
TEST_DIR"/error/func/.shadowed_param.sk.out"
);


pass |= !e2e_wrapper(
"f() {\n" \
"	return 0\n" \
"}\n" \
"\n" \
"f()\n",
TEST_DIR"/error/func/unexpected_return.sk",
TEST_DIR"/error/func/.unexpected_return.sk.out"
);


pass |= !e2e_wrapper(
"x := 1\n" \
"\n" \
"x() { noop }\n",
TEST_DIR"/error/func/var_as_func.sk",
TEST_DIR"/error/func/.var_as_func.sk.out"
);


pass |= !e2e_wrapper(
"external f()\n" \
"\n" \
"x := f(1)\n",
TEST_DIR"/error/func/var_def_check_func.sk",
TEST_DIR"/error/func/.var_def_check_func.sk.out"
);


pass |= !e2e_wrapper(
"x := Int\n" \
"x := 1\n",
TEST_DIR"/error/misc/alias_as_var.sk",
TEST_DIR"/error/misc/.alias_as_var.sk.out"
);


pass |= !e2e_wrapper(
"#{\n",
TEST_DIR"/error/misc/closing_block_comment_missing.sk",
TEST_DIR"/error/misc/.closing_block_comment_missing.sk.out"
);


pass |= !e2e_wrapper(
"if true {\n" \
"	return 1\n" \
"} {\n" \
"	return 2\n" \
"}\n" \
"\n" \
"return 0\n",
TEST_DIR"/error/misc/duplicated_code_block.sk",
TEST_DIR"/error/misc/.duplicated_code_block.sk.out"
);


pass |= !e2e_wrapper(
"if true { return 1 }}\n",
TEST_DIR"/error/misc/extra_close_bracket.sk",
TEST_DIR"/error/misc/.extra_close_bracket.sk.out"
);


pass |= !e2e_wrapper(
"\xc3\x28\n",
TEST_DIR"/error/misc/illegal_utf8.sk",
TEST_DIR"/error/misc/.illegal_utf8.sk.out"
);


pass |= !e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 {\n" \
"	# invalid\n" \
"}\n",
TEST_DIR"/error/misc/invalid_block_in_assign.sk",
TEST_DIR"/error/misc/.invalid_block_in_assign.sk.out"
);


pass |= !e2e_wrapper(
"x := 1\n" \
"{\n" \
"	# invalid\n" \
"}\n",
TEST_DIR"/error/misc/invalid_block_in_def.sk",
TEST_DIR"/error/misc/.invalid_block_in_def.sk.out"
);


pass |= !e2e_wrapper(
"return 1 {\n" \
"	# invalid\n" \
"}\n",
TEST_DIR"/error/misc/invalid_block_in_return.sk",
TEST_DIR"/error/misc/.invalid_block_in_return.sk.out"
);


pass |= !e2e_wrapper(
"#x\n",
TEST_DIR"/error/misc/invalid_comment_start.sk",
TEST_DIR"/error/misc/.invalid_comment_start.sk.out"
);


pass |= !e2e_wrapper(
"x!@$ := \"this will fail\"\n",
TEST_DIR"/error/misc/invalid_identifier.sk",
TEST_DIR"/error/misc/.invalid_identifier.sk.out"
);


pass |= !e2e_wrapper(
"x := (_)\n",
TEST_DIR"/error/misc/invalid_paren_expr.sk",
TEST_DIR"/error/misc/.invalid_paren_expr.sk.out"
);


pass |= !e2e_wrapper(
"x := '\\xdb80'\n",
TEST_DIR"/error/misc/invalid_utf_rune.sk",
TEST_DIR"/error/misc/.invalid_utf_rune.sk.out"
);


pass |= !e2e_wrapper(
"x := \"\\xdb80\"\n",
TEST_DIR"/error/misc/invalid_utf_str.sk",
TEST_DIR"/error/misc/.invalid_utf_str.sk.out"
);


pass |= !e2e_wrapper(
"export main() {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/misc/main_reserved.sk",
TEST_DIR"/error/misc/.main_reserved.sk.out"
);


pass |= !e2e_wrapper(
"if true {{\n" \
"	return 1\n" \
"}\n" \
"\n" \
"return 0\n",
TEST_DIR"/error/misc/mismatched_bracket.sk",
TEST_DIR"/error/misc/.mismatched_bracket.sk.out"
);


pass |= !e2e_wrapper(
"x()\n",
TEST_DIR"/error/misc/missing_external.sk",
TEST_DIR"/error/misc/.missing_external.sk.out"
);


pass |= !e2e_wrapper(
"x := \"\n",
TEST_DIR"/error/misc/missing_quote.sk",
TEST_DIR"/error/misc/.missing_quote.sk.out"
);


pass |= !e2e_wrapper(
"{\n",
TEST_DIR"/error/misc/no_closing_bracket.sk",
TEST_DIR"/error/misc/.no_closing_bracket.sk.out"
);


pass |= !e2e_wrapper(
"if false {\n" \
"	export func() {\n" \
		"# this will fail\n" \
"	}\n" \
"}\n" \
"\n" \
"func()\n",
TEST_DIR"/error/misc/no_nested_func.sk",
TEST_DIR"/error/misc/.no_nested_func.sk.out"
);


pass |= !e2e_wrapper(
"external x() x()\n",
TEST_DIR"/error/misc/no_newline_external.sk",
TEST_DIR"/error/misc/.no_newline_external.sk.out"
);


pass |= !e2e_wrapper(
"some_alias := Int\n" \
"some_alias := Float\n",
TEST_DIR"/error/misc/no_same_name_aliases.sk",
TEST_DIR"/error/misc/.no_same_name_aliases.sk.out"
);


pass |= !e2e_wrapper(
"{ noop }\n",
TEST_DIR"/error/misc/only_brackets.sk",
TEST_DIR"/error/misc/.only_brackets.sk.out"
);


pass |= !e2e_wrapper(
"x\n",
TEST_DIR"/error/misc/only_identifier_fails.sk",
TEST_DIR"/error/misc/.only_identifier_fails.sk.out"
);


pass |= !e2e_wrapper(
"x := (1\n",
TEST_DIR"/error/misc/paren_expr_missing_paren.sk",
TEST_DIR"/error/misc/.paren_expr_missing_paren.sk.out"
);


pass |= !e2e_wrapper(
"}\n",
TEST_DIR"/error/misc/stray_close_bracket.sk",
TEST_DIR"/error/misc/.stray_close_bracket.sk.out"
);


pass |= !e2e_wrapper(
"x := \"\n" \
"\n" \
"\n" \
"\"\n" \
"\n" \
"#{\n" \
"\n" \
"some comment\n" \
"\n" \
"#}\n" \
"\n" \
"invalid_token\n",
TEST_DIR"/error/misc/token_iter_keep_loc.sk",
TEST_DIR"/error/misc/.token_iter_keep_loc.sk.out"
);


pass |= !e2e_wrapper(
"# TODO: fix semantic layer not checking for trivial types\n" \
"\n" \
"x: Int = 1\n" \
"\n" \
"# explicitly cause file to fail so CI passes\n" \
"y := x + false\n",
TEST_DIR"/error/misc/trivial_type.sk",
TEST_DIR"/error/misc/.trivial_type.sk.out"
);


pass |= !e2e_wrapper(
"# do nothing\n" \
"\n" \
"{\n" \
"	# not allowed\n" \
"}\n",
TEST_DIR"/error/misc/unexpected_code_block.sk",
TEST_DIR"/error/misc/.unexpected_code_block.sk.out"
);


pass |= !e2e_wrapper(
"\"this will fail\"\n",
TEST_DIR"/error/misc/unexpected_str_fails.sk",
TEST_DIR"/error/misc/.unexpected_str_fails.sk.out"
);


pass |= !e2e_wrapper(
"this_will_fail\n",
TEST_DIR"/error/misc/unexpected_token_fails.sk",
TEST_DIR"/error/misc/.unexpected_token_fails.sk.out"
);


pass |= !e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 + \"fail\"\n",
TEST_DIR"/error/oper/add_mismatched_consts.sk",
TEST_DIR"/error/oper/.add_mismatched_consts.sk.out"
);


pass |= !e2e_wrapper(
"x: Float = 1\n",
TEST_DIR"/error/oper/assign_bad_type.sk",
TEST_DIR"/error/oper/.assign_bad_type.sk.out"
);


pass |= !e2e_wrapper(
"mut x := 1 and 1\n",
TEST_DIR"/error/oper/bool_expr_and_not_bool.sk",
TEST_DIR"/error/oper/.bool_expr_and_not_bool.sk.out"
);


pass |= !e2e_wrapper(
"mut x := 1 or 1\n",
TEST_DIR"/error/oper/bool_expr_or_not_bool.sk",
TEST_DIR"/error/oper/.bool_expr_or_not_bool.sk.out"
);


pass |= !e2e_wrapper(
"mut x := 1 xor 1\n",
TEST_DIR"/error/oper/bool_expr_xor_not_bool.sk",
TEST_DIR"/error/oper/.bool_expr_xor_not_bool.sk.out"
);


pass |= !e2e_wrapper(
"x := 1 / 0\n",
TEST_DIR"/error/oper/div_by_zero.sk",
TEST_DIR"/error/oper/.div_by_zero.sk.out"
);


pass |= !e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 / \"fail\"\n",
TEST_DIR"/error/oper/div_mismatched_consts.sk",
TEST_DIR"/error/oper/.div_mismatched_consts.sk.out"
);


pass |= !e2e_wrapper(
"x := 0\n" \
"\n" \
"z := x + 1.0\n",
TEST_DIR"/error/oper/lhs_var_bad_type.sk",
TEST_DIR"/error/oper/.lhs_var_bad_type.sk.out"
);


pass |= !e2e_wrapper(
"x := fail + 1\n",
TEST_DIR"/error/oper/lhs_var_not_found.sk",
TEST_DIR"/error/oper/.lhs_var_not_found.sk.out"
);


pass |= !e2e_wrapper(
"x := x < 1\n",
TEST_DIR"/error/oper/lhs_var_self_ref.sk",
TEST_DIR"/error/oper/.lhs_var_self_ref.sk.out"
);


pass |= !e2e_wrapper(
"x := 1 +\n",
TEST_DIR"/error/oper/missing_rhs.sk",
TEST_DIR"/error/oper/.missing_rhs.sk.out"
);


pass |= !e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 * \"fail\"\n",
TEST_DIR"/error/oper/mult_mismatched_consts.sk",
TEST_DIR"/error/oper/.mult_mismatched_consts.sk.out"
);


pass |= !e2e_wrapper(
"x := 0\n" \
"\n" \
"if x {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/oper/non_bool_cond.sk",
TEST_DIR"/error/oper/.non_bool_cond.sk.out"
);


pass |= !e2e_wrapper(
"x := \"123\" ^ \"123\"\n",
TEST_DIR"/error/oper/pow_bad_type.sk",
TEST_DIR"/error/oper/.pow_bad_type.sk.out"
);


pass |= !e2e_wrapper(
"mut x := 1\n" \
"\n" \
"x = 1.0\n",
TEST_DIR"/error/oper/reassign_different_type.sk",
TEST_DIR"/error/oper/.reassign_different_type.sk.out"
);


pass |= !e2e_wrapper(
"x := 0\n" \
"\n" \
"z := 1.0 + x\n",
TEST_DIR"/error/oper/rhs_var_bad_type.sk",
TEST_DIR"/error/oper/.rhs_var_bad_type.sk.out"
);


pass |= !e2e_wrapper(
"x := 1 + fail\n",
TEST_DIR"/error/oper/rhs_var_not_found.sk",
TEST_DIR"/error/oper/.rhs_var_not_found.sk.out"
);


pass |= !e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 - \"fail\"\n",
TEST_DIR"/error/oper/sub_mismatched_consts.sk",
TEST_DIR"/error/oper/.sub_mismatched_consts.sk.out"
);


pass |= !e2e_wrapper(
"x := 1\n" \
"x\n",
TEST_DIR"/error/oper/trailing_token.sk",
TEST_DIR"/error/oper/.trailing_token.sk.out"
);


pass |= !e2e_wrapper(
"mut x := not 1\n",
TEST_DIR"/error/oper/using_non_bool_with_not.sk",
TEST_DIR"/error/oper/.using_non_bool_with_not.sk.out"
);

	return pass;
}
