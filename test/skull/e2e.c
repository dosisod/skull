#include <stdbool.h>
#include <string.h>

#include "skull/codegen/entry.h"
#include "skull/common/local.h"

bool SKULL_TESTING = 1;
#define TEST_DIR "./test/sh"

int main(void) {
setup_locale();

bool pass = false;

char *code = strdup(
	"mut bool := true\n" \
	"mut int := 1234\n" \
	"mut float := 3.14\n" \
	"mut str := \"hello world\"\n" \
	"mut rune := 'x'\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/basic_types.sk", code);


code = strdup(
	"mut x := true and true\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/bool_expr/and.sk", code);


code = strdup(
	"mut x := 1 >= 1\n" \
	"mut y := 1.0 >= 1.0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/bool_expr/gtr_than_eq.sk", code);


code = strdup(
	"mut x := 2 > 1\n" \
	"mut y := 2.0 > 1.0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/bool_expr/gtr_than.sk", code);


code = strdup(
	"mut x := 1 isnt 2\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/bool_expr/isnt.sk", code);


code = strdup(
	"mut x := 1 is 1\n" \
	"mut y := 1.0 is 1.0\n" \
	"mut z := 'x' is 'x'\n" \
	"mut q := true is true\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/bool_expr/is.sk", code);


code = strdup(
	"mut x := 1 <= 1\n" \
	"mut y := 1.0 <= 1.0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/bool_expr/less_than_eq.sk", code);


code = strdup(
	"mut x := 1 < 2\n" \
	"mut y := 1.0 < 2.0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/bool_expr/less_than.sk", code);


code = strdup(
	"mut x := not false\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/bool_expr/not.sk", code);


code = strdup(
	"mut x := false or true\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/bool_expr/or.sk", code);


code = strdup(
	"x := 1\n" \
	"\n" \
	"y := 1 is x\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/bool_expr/var_rhs.sk", code);


code = strdup(
	"mut x := false xor true\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/bool_expr/xor.sk", code);


code = strdup(
	"export x := 1\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/export_var.sk", code);


code = strdup(
	"mut x := 0\n" \
	"mut y := 0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/many_vars.sk", code);


code = strdup(
	"mut x: Int = 0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/mut_with_explicit_type.sk", code);


code = strdup(
	"mut x := \"abc\"\n" \
	"x = \"defg\"\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/reassign_str.sk", code);


code = strdup(
	"mut x := 0\n" \
	"\n" \
	"x = 1\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/reassign_var.sk", code);


code = strdup(
	"x := \"# \"\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/str_with_comment.sk", code);


code = strdup(
	"mut x := 0\n" \
	"mut z := x\n" \
	"\n" \
	"return z\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/declare/variable_auto_deduce.sk", code);


code = strdup(
	"if false {\n" \
		"noop\n" \
	"}\n" \
	"elif true {\n" \
		"noop\n" \
	"}\n" \
	"else {\n" \
		"noop\n" \
	"}\n" \
	"\n" \
	"f() Bool { return false }\n" \
	"\n" \
	"if f() {\n" \
		"noop\n" \
	"}\n" \
	"elif f() {\n" \
		"noop\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/elif.sk", code);


code = strdup(
	"if true {\n" \
		"# do nothing\n" \
	"}\n" \
	"else {\n" \
		"# do nothing\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/else.sk", code);


code = strdup(
	"if true {\n" \
		"return 1\n" \
	"}\n" \
	"# comment\n" \
	"else {\n" \
		"return 2\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/if/else_comment.sk", code);


code = strdup(
	"if false {\n" \
	  "return 0\n" \
	"}\n" \
	"\n" \
	"return 1\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/if/false.sk", code);


code = strdup(
	"if not true {\n" \
		"# do nothing\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/if/not_with_const.sk", code);


code = strdup(
	"mut x := true\n" \
	"\n" \
	"if not x {\n" \
		"# do nothing\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/if/not_with_var.sk", code);


code = strdup(
	"if true {\n" \
	  "return 0\n" \
	"}\n" \
	"\n" \
	"return 1\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/if/true.sk", code);


code = strdup(
	"mut x := false\n" \
	"\n" \
	"if x {\n" \
		"return 1\n" \
	"}\n" \
	"\n" \
	"return 0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/if/with_var_false.sk", code);


code = strdup(
	"mut x := true\n" \
	"\n" \
	"if x {\n" \
		"return 1\n" \
	"}\n" \
	"\n" \
	"return 0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/if/with_var_true.sk", code);


code = strdup(
	"if true {\n" \
		"if true {\n" \
			"return 1\n" \
		"}\n" \
	"}\n" \
	"\n" \
	"return 0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/nested_if.sk", code);


code = strdup(
	"return 0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/return_0.sk", code);


code = strdup(
	"return 1 + 2\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/return_expr.sk", code);


code = strdup(
	"external f() Int\n" \
	"\n" \
	"return f()\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/return_func_value.sk", code);


code = strdup(
	"mut x := 1\n" \
	"\n" \
	"return x\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/return_var.sk", code);


code = strdup(
	"if true {\n" \
		"mut x := 1\n" \
	"}\n" \
	"\n" \
	"return 0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/set_var_in_if.sk", code);


code = strdup(
	"return 0\n" \
	"unreachable\n" \
	"# some comment\n" \
	"noop\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/unreachable_ignored.sk", code);


code = strdup(
	"f() {\n" \
		"unreachable\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/unreachable_in_func.sk", code);


code = strdup(
	"while true {\n" \
		"# do nothing\n" \
	"}\n" \
	"\n" \
	"i := 0\n" \
	"\n" \
	"while i < 10 {\n" \
		"noop\n" \
	"}\n" \
	"\n" \
	"f() Bool { return true }\n" \
	"\n" \
	"while f() {\n" \
		"noop\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/flow/while.sk", code);


code = strdup(
	"external func() Int\n" \
	"\n" \
	"mut x := func()\n" \
	"\n" \
	"return x\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/assign_return_value.sk", code);


code = strdup(
	"external puts(s: Str)\n" \
	"\n" \
	"puts(\"hello world!\")\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/call_str.sk", code);


code = strdup(
	"f() {\n" \
		"# do nothing\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/declare_func.sk", code);


code = strdup(
	"x() { noop }\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/empty_func_no_err.sk", code);


code = strdup(
	"export f() {\n" \
		"# do nothing\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/exported.sk", code);


code = strdup(
	"f(num: Int) {\n" \
		"# do nothing\n" \
	"}\n" \
	"\n" \
	"f(1 + 2)\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/expr_param.sk", code);


code = strdup(
	"external x() Int\n" \
	"external y(num: Int) Int\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/external_return_type.sk", code);


code = strdup(
	"external x(a: Int)\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/external_with_param.sk", code);


code = strdup(
	"f(x: Int) Int {\n" \
		"return x\n" \
	"}\n" \
	"\n" \
	"f(f(1))\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/func_expr_param.sk", code);


code = strdup(
	"f() {\n" \
		"mut x := 1\n" \
	"}\n" \
	"\n" \
	"mut x := 0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/func_with_scope.sk", code);


code = strdup(
	"f(a: Int, b: Float) {\n" \
		"# do nothing\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/multi_param_func_different_types.sk", code);


code = strdup(
	"f(a: Int, b: Int) Int {\n" \
		"x := a + b\n" \
	"\n" \
		"return x\n" \
	"}\n" \
	"\n" \
	"f(1, 2)\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/multi_param_func.sk", code);


code = strdup(
	"external a()\n" \
	"external b()\n" \
	"\n" \
	"a()\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/multiple_externals.sk", code);


code = strdup(
	"f(num: Int) Int {\n" \
		"return num\n" \
	"}\n" \
	"\n" \
	"f(1)\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/multi_var_func.sk", code);


code = strdup(
	"f() {\n" \
		"if true {\n" \
			"# do nothing\n" \
		"}\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/nested_if.sk", code);


code = strdup(
	"external x()\n" \
	"\n" \
	"x()\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/no_args.sk", code);


code = strdup(
	"external x()\n" \
	"\n" \
	"x()\n" \
	"\n" \
	"return 1\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/nodes_after_call_not_ignored.sk", code);


code = strdup(
	"func() Float {\n" \
		"mut ret := 3.14\n" \
		"return ret\n" \
	"}\n" \
	"\n" \
	"func2() Float {\n" \
		"return 3.14\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/non_int_return.sk", code);


code = strdup(
	"external x(a: Int)\n" \
	"\n" \
	"x(1)\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/single_param.sk", code);


code = strdup(
	"external f(x: Int)\n" \
	"\n" \
	"mut num := 1234\n" \
	"\n" \
	"f(num)\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/var_param.sk", code);


code = strdup(
	"f() {\n" \
		"return\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/function/void_explicit_ret.sk", code);


code = strdup(
	"# comment\n" \
	"x := 0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/comment_then_code.sk", code);


code = strdup(
	"x := 1\n" \
	"\n" \
	"y := x + 1\n" \
	"\n" \
	"z := y + x\n" \
	"\n" \
	"return z\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/const_inline.sk", code);


code = strdup(
	"mut x := \"\\n\"\n" \
	"mut y := '\\n'\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/escape_sequences.sk", code);


code = strdup(
	"mut is_this_kept := true\n" \
	"#{\n" \
	"mut and_this := false\n" \
	"#}\n" \
	"mut what_about_this := true\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/ignore_block_comments.sk", code);


code = strdup(
	"# comment will not cause any issues\n" \
	"\n" \
	"return 0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/ignore_comments.sk", code);


code = strdup(
	"emoji := \"😀\"\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/locale.sk", code);


code = strdup(
	"#{\n" \
	"#{\n" \
	"#}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/nested_block_comment.sk", code);


code = strdup(
	"return 0"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/no_eol_works.sk", code);


code = strdup(
	"if true {\n" \
		"noop\n" \
	"}\n" \
	"\n" \
	"noop\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/noop.sk", code);


code = strdup(
	"if false {\n" \
		"x := Int\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/not_top_lvl_alias.sk", code);


code = strdup(
	"x := 0\n" \
	"\n" \
	"if true {\n" \
		"if true {\n" \
			"# do nothing\n" \
		"}\n" \
	"\n" \
		"z := x\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/preserve_nested_scopes.sk", code);


code = strdup(
	"f(a: Int, b: Int, c: Int) { noop }\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/triple_param_no_overflow.sk", code);


code = strdup(
	"int_alias := Int\n" \
	"\n" \
	"mut x: int_alias = 1234\n" \
	"\n" \
	"f(num: int_alias) int_alias {\n" \
		"return num\n" \
	"}\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/misc/type_alias.sk", code);


code = strdup(
	"mut x := 2.0 + 3.0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/add_float_consts.sk", code);


code = strdup(
	"mut x := 1 + 2\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/add_int_consts.sk", code);


code = strdup(
	"mut x := 4.0 / 2.0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/div_float_consts.sk", code);


code = strdup(
	"mut x := 4 / 2\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/div_int_consts.sk", code);


code = strdup(
	"external f() Int\n" \
	"\n" \
	"x := f() / f()\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/div_with_expr.sk", code);


code = strdup(
	"mut x := 2.0 ^ 3.0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/float_pow.sk", code);


code = strdup(
	"external f() Int\n" \
	"\n" \
	"mut x := f() + 1\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/func_lhs.sk", code);


code = strdup(
	"external f() Int\n" \
	"\n" \
	"mut x := 1 + f()\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/func_rhs.sk", code);


code = strdup(
	"mut x := 10 ^ 3\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/int_pow.sk", code);


code = strdup(
	"mut x := 3.14 is 3.14\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/is_cmp_float.sk", code);


code = strdup(
	"mut x := 1234 is 1234\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/is_cmp_int.sk", code);


code = strdup(
	"mut x := 'x' is 'x'\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/is_cmp_rune.sk", code);


code = strdup(
	"mut x := \"x\" is \"x\"\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/is_cmp_str.sk", code);


code = strdup(
	"mut x := 1\n" \
	"mut y := x + 2\n" \
	"mut z := x < 1\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/lhs_with_var.sk", code);


code = strdup(
	"mut x := 1 << 2\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/lshift.sk", code);


code = strdup(
	"mut x := 9 mod 2\n" \
	"mut y := -10 mod 3\n" \
	"mut z := 12.0 mod 5.0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/mod.sk", code);


code = strdup(
	"mut x := 2.0 * 3.0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/mult_float_consts.sk", code);


code = strdup(
	"mut x := 2 * 3\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/mult_int_consts.sk", code);


code = strdup(
	"x := 1\n" \
	"\n" \
	"return - x\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/negative_unary.sk", code);


code = strdup(
	"mut x := (1 + 2)\n" \
	"\n" \
	"mut y := (((1 + 2)))\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/paren_expr.sk", code);


code = strdup(
	"mut x := 1 + (2 + 3)\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/paren_rhs_expr.sk", code);


code = strdup(
	"mut x := 1\n" \
	"\n" \
	"mut z := 2 + x\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/rhs_with_var.sk", code);


code = strdup(
	"return 0b1111 >> 2\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/rshift.sk", code);


code = strdup(
	"mut x := 2.0 - 3.0\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/sub_float_consts.sk", code);


code = strdup(
	"mut x := 1 - 2\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/sub_int_consts.sk", code);


code = strdup(
	"mut w := - 1.0\n" \
	"mut x := not not false\n" \
	"mut y := - 1\n" \
	"mut z := - y\n"
);
pass |= init_codegen_pipeline(TEST_DIR"/oper/unary_expr.sk", code);


	return pass;
}
