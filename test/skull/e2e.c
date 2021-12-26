#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/common/color.h"
#include "skull/common/errors.h"
#include "skull/common/local.h"
#include "skull/common/vector.h"
#include "skull/pipeline.h"
#include "test/testing.h"

#define TEST_DIR "./test/sh"

static int e2e_wrapper(const char *, const char *, const char *, const char *);
static char *mock_file_to_output_file(const char *, const char *);
static bool compare_compiler_output(const char *, const char *);

int main(void) {
	setup_locale();

	bool pass = false;

pass |= e2e_wrapper(
"mut bool := true\n" \
"mut int := 1234\n" \
"mut float := 3.14\n" \
"mut str := \"hello world\"\n" \
"mut rune := 'x'\n" \
"\n" \
"mut not_ := not false\n" \
"mut and_ := true and true\n" \
"mut or_ := false or true\n" \
"mut xor_ := false xor true\n" \
"\n" \
"mut greater_than_eq_int := 1 >= 1\n" \
"mut greater_than_eq_float := 1.0 >= 1.0\n" \
"\n" \
"mut gtr_than_int := 2 > 1\n" \
"mut gtr_than_float := 2.0 > 1.0\n" \
"\n" \
"mut is_int := 1 is 1\n" \
"mut is_float := 1.0 is 1.0\n" \
"mut is_rune := 'x' is 'x'\n" \
"mut is_bool := true is true\n" \
"\n" \
"mut isnt_int := 1 isnt 1\n" \
"mut isnt_float := 1.0 isnt 1.0\n" \
"mut isnt_rune := 'x' isnt 'x'\n" \
"mut isnt_bool := true isnt true\n" \
"\n" \
"mut less_than_eq_int := 1 <= 1\n" \
"mut less_than_eq_float := 1.0 <= 1.0\n" \
"\n" \
"mut less_than_int := 1 < 2\n" \
"mut less_than_float := 1.0 < 2.0\n" \
"\n" \
"mut mult_float := 2.0 * 3.0\n" \
"mut mult_int := 2 * 3\n" \
"\n" \
"mut div_float := 4.0 / 2.0\n" \
"mut div_int := 4 / 2\n" \
"\n" \
"mut add_int := 1 + 2\n" \
"mut add_float := 2.0 + 3.0\n" \
"\n" \
"mut subtract_int := 1 - 2\n" \
"mut subtract_float := 2.0 - 3.0\n" \
"\n" \
"mut lshift := 1 << 2\n" \
"mut rshift := 0b1111 >> 2\n" \
"\n" \
"mut mod_int := 9 mod 2\n" \
"mut mod_negative_int := -10 mod 3\n" \
"mut mod_float := 12.0 mod 5.0\n" \
"\n",

TEST_DIR"/oper/all_exprs.sk",

"; ModuleID = './test/sh/oper/all_exprs.sk'\n" \
"source_filename = \"./test/sh/oper/all_exprs.sk\"\n" \
"\n" \
"@bool = private global i1 true\n" \
"@int = private global i64 1234\n" \
"@float = private global double 3.140000e+00\n" \
"@0 = private unnamed_addr constant [12 x i8] c\"hello world\00\", align 1\n" \
"@str = private global i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0)\n" \
"@rune = private global i32 120\n" \
"@not_ = private global i1 true\n" \
"@and_ = private global i1 true\n" \
"@or_ = private global i1 true\n" \
"@xor_ = private global i1 true\n" \
"@greater_than_eq_int = private global i1 true\n" \
"@greater_than_eq_float = private global i1 true\n" \
"@gtr_than_int = private global i1 true\n" \
"@gtr_than_float = private global i1 true\n" \
"@is_int = private global i1 true\n" \
"@is_float = private global i1 true\n" \
"@is_rune = private global i1 true\n" \
"@is_bool = private global i1 true\n" \
"@isnt_int = private global i1 false\n" \
"@isnt_float = private global i1 false\n" \
"@isnt_rune = private global i1 false\n" \
"@isnt_bool = private global i1 false\n" \
"@less_than_eq_int = private global i1 true\n" \
"@less_than_eq_float = private global i1 true\n" \
"@less_than_int = private global i1 true\n" \
"@less_than_float = private global i1 true\n" \
"@mult_float = private global double 6.000000e+00\n" \
"@mult_int = private global i64 6\n" \
"@div_float = private global double 2.000000e+00\n" \
"@div_int = private global i64 2\n" \
"@add_int = private global i64 3\n" \
"@add_float = private global double 5.000000e+00\n" \
"@subtract_int = private global i64 -1\n" \
"@subtract_float = private global double -1.000000e+00\n" \
"@lshift = private global i64 4\n" \
"@rshift = private global i64 3\n" \
"@mod_int = private global i64 1\n" \
"@mod_negative_int = private global i64 -1\n" \
"@mod_float = private global double 2.000000e+00\n" \
"\n" \
"define i64 @.tmp() {\n" \
"entry:\n" \
  "ret i64 0\n" \
"}\n",

NULL
);

pass |= e2e_wrapper( //
"	mut bool := true\n" \
"	mut int := 1234\n" \
"	mut float := 3.14\n" \
"	mut str := \"hello world\"\n" \
"	mut rune := 'x'\n",

TEST_DIR"/declare/basic_types.sk",

"; ModuleID = './test/sh/declare/basic_types.sk'\n" \
"source_filename = \"./test/sh/declare/basic_types.sk\"\n" \
"\n" \
"@bool = private global i1 true\n" \
"@int = private global i64 1234\n" \
"@float = private global double 3.140000e+00\n" \
"@0 = private unnamed_addr constant [12 x i8] c\"hello world\\00\", align 1\n" \
"@str = private global i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0)\n" \
"@rune = private global i32 120\n" \
"\n" \
"define i64 @.basic_types() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	x := 1\n" \
"	\n" \
"	y := 1 is x\n",

TEST_DIR"/declare/bool_expr/var_rhs.sk",

"; ModuleID = './test/sh/declare/bool_expr/var_rhs.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/var_rhs.sk\"\n" \
"\n" \
"define i64 @.var_rhs() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	export x := 1\n",

TEST_DIR"/declare/export_var.sk",

"; ModuleID = './test/sh/declare/export_var.sk'\n" \
"source_filename = \"./test/sh/declare/export_var.sk\"\n" \
"\n" \
"@x = global i64 1\n" \
"\n" \
"define i64 @.export_var() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := 0\n" \
"	mut y := 0\n",

TEST_DIR"/declare/many_vars.sk",

"; ModuleID = './test/sh/declare/many_vars.sk'\n" \
"source_filename = \"./test/sh/declare/many_vars.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"@y = private global i64 0\n" \
"\n" \
"define i64 @.many_vars() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut x: Int = 0\n",

TEST_DIR"/declare/mut_with_explicit_type.sk",

"; ModuleID = './test/sh/declare/mut_with_explicit_type.sk'\n" \
"source_filename = \"./test/sh/declare/mut_with_explicit_type.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.mut_with_explicit_type() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := \"abc\"\n" \
"	x = \"defg\"\n",

TEST_DIR"/declare/reassign_str.sk",

"; ModuleID = './test/sh/declare/reassign_str.sk'\n" \
"source_filename = \"./test/sh/declare/reassign_str.sk\"\n" \
"\n" \
"@0 = private unnamed_addr constant [4 x i8] c\"abc\\00\", align 1\n" \
"@x = private global i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0)\n" \
"@1 = private unnamed_addr constant [5 x i8] c\"defg\\00\", align 1\n" \
"\n" \
"define i64 @.reassign_str() {\n" \
"entry:\n" \
"  store i8* getelementptr inbounds ([5 x i8], [5 x i8]* @1, i32 0, i32 0), i8** @x, align 8\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := 0\n" \
"	\n" \
"	x = 1\n",

TEST_DIR"/declare/reassign_var.sk",

"; ModuleID = './test/sh/declare/reassign_var.sk'\n" \
"source_filename = \"./test/sh/declare/reassign_var.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.reassign_var() {\n" \
"entry:\n" \
"  store i64 1, i64* @x, align 4\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	x := \"# \"\n",

TEST_DIR"/declare/str_with_comment.sk",

"; ModuleID = './test/sh/declare/str_with_comment.sk'\n" \
"source_filename = \"./test/sh/declare/str_with_comment.sk\"\n" \
"\n" \
"@0 = private unnamed_addr constant [3 x i8] c\"# \\00\", align 1\n" \
"\n" \
"define i64 @.str_with_comment() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := 0\n" \
"	mut z := x\n" \
"	\n" \
"	return z\n",

TEST_DIR"/declare/variable_auto_deduce.sk",

"; ModuleID = './test/sh/declare/variable_auto_deduce.sk'\n" \
"source_filename = \"./test/sh/declare/variable_auto_deduce.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"@z = private global i64 0\n" \
"\n" \
"define i64 @.variable_auto_deduce() {\n" \
"entry:\n" \
"  %0 = load i64, i64* @x, align 4\n" \
"  store i64 %0, i64* @z, align 4\n" \
"  %1 = load i64, i64* @z, align 4\n" \
"  ret i64 %1\n" \
"}\n",

NULL
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

"; ModuleID = './test/sh/flow/elif.sk'\n" \
"source_filename = \"./test/sh/flow/elif.sk\"\n" \
"\n" \
"define i64 @.elif() {\n" \
"entry:\n" \
"  br i1 false, label %if_true, label %if_false\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  br label %end\n" \
"\n" \
"if_false:                                         ; preds = %entry\n" \
"  br i1 true, label %if_true1, label %if_false2\n" \
"\n" \
"if_true1:                                         ; preds = %if_false\n" \
"  br label %end\n" \
"\n" \
"if_false2:                                        ; preds = %if_false\n" \
"  br label %end\n" \
"\n" \
"end:                                              ; preds = %if_false2, %if_true1, %if_true\n" \
"  %0 = call i1 @f()\n" \
"  br i1 %0, label %if_true4, label %if_false5\n" \
"\n" \
"if_true4:                                         ; preds = %end\n" \
"  br label %end3\n" \
"\n" \
"if_false5:                                        ; preds = %end\n" \
"  %1 = call i1 @f()\n" \
"  br i1 %1, label %if_true6, label %end3\n" \
"\n" \
"if_true6:                                         ; preds = %if_false5\n" \
"  br label %end3\n" \
"\n" \
"end3:                                             ; preds = %if_false5, %if_true6, %if_true4\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private i1 @f() {\n" \
"entry:\n" \
"  ret i1 false\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	if true {\n" \
		"# do nothing\n" \
"	}\n" \
"	else {\n" \
		"# do nothing\n" \
"	}\n",

TEST_DIR"/flow/else.sk",

"; ModuleID = './test/sh/flow/else.sk'\n" \
"source_filename = \"./test/sh/flow/else.sk\"\n" \
"\n" \
"define i64 @.else() {\n" \
"entry:\n" \
"  br i1 true, label %if_true, label %if_false\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  br label %end\n" \
"\n" \
"if_false:                                         ; preds = %entry\n" \
"  br label %end\n" \
"\n" \
"end:                                              ; preds = %if_false, %if_true\n" \
"  ret i64 0\n" \
"}\n",

NULL
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

"; ModuleID = './test/sh/flow/if/else_comment.sk'\n" \
"source_filename = \"./test/sh/flow/if/else_comment.sk\"\n" \
"\n" \
"define i64 @.else_comment() {\n" \
"entry:\n" \
"  br i1 true, label %if_true, label %if_false\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  ret i64 1\n" \
"\n" \
"if_false:                                         ; preds = %entry\n" \
"  ret i64 2\n" \
"\n" \
"end:                                              ; No predecessors!\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := true\n" \
"	\n" \
"	if not x {\n" \
		"# do nothing\n" \
"	}\n",

TEST_DIR"/flow/if/not_with_var.sk",

"; ModuleID = './test/sh/flow/if/not_with_var.sk'\n" \
"source_filename = \"./test/sh/flow/if/not_with_var.sk\"\n" \
"\n" \
"@x = private global i1 true\n" \
"\n" \
"define i64 @.not_with_var() {\n" \
"entry:\n" \
"  %0 = load i1, i1* @x, align 1\n" \
"  %1 = xor i1 %0, true\n" \
"  br i1 %1, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  br label %end\n" \
"\n" \
"end:                                              ; preds = %entry, %if_true\n" \
"  ret i64 0\n" \
"}\n",

NULL
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

"; ModuleID = './test/sh/flow/if/with_var_true.sk'\n" \
"source_filename = \"./test/sh/flow/if/with_var_true.sk\"\n" \
"\n" \
"@x = private global i1 true\n" \
"\n" \
"define i64 @.with_var_true() {\n" \
"entry:\n" \
"  %0 = load i1, i1* @x, align 1\n" \
"  br i1 %0, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  ret i64 1\n" \
"\n" \
"end:                                              ; preds = %entry\n" \
"  ret i64 0\n" \
"}\n",

NULL
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

"; ModuleID = './test/sh/flow/nested_if.sk'\n" \
"source_filename = \"./test/sh/flow/nested_if.sk\"\n" \
"\n" \
"define i64 @.nested_if() {\n" \
"entry:\n" \
"  br i1 true, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  br i1 true, label %if_true2, label %end1\n" \
"\n" \
"end:                                              ; preds = %entry, %end1\n" \
"  ret i64 0\n" \
"\n" \
"if_true2:                                         ; preds = %if_true\n" \
"  ret i64 1\n" \
"\n" \
"end1:                                             ; preds = %if_true\n" \
"  br label %end\n" \
"}\n",

NULL
);



pass |= e2e_wrapper(
"	return 1 + 2\n",

TEST_DIR"/flow/return_expr.sk",

"; ModuleID = './test/sh/flow/return_expr.sk'\n" \
"source_filename = \"./test/sh/flow/return_expr.sk\"\n" \
"\n" \
"define i64 @.return_expr() {\n" \
"entry:\n" \
"  ret i64 3\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	if true {\n" \
		"mut x := 1\n" \
"	}\n" \
"	\n" \
"	return 0\n",

TEST_DIR"/flow/set_var_in_if.sk",

"; ModuleID = './test/sh/flow/set_var_in_if.sk'\n" \
"source_filename = \"./test/sh/flow/set_var_in_if.sk\"\n" \
"\n" \
"@x = private global i64 1\n" \
"\n" \
"define i64 @.set_var_in_if() {\n" \
"entry:\n" \
"  br i1 true, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  br label %end\n" \
"\n" \
"end:                                              ; preds = %entry, %if_true\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	return 0\n" \
"	unreachable\n" \
"	# some comment\n" \
"	noop\n",

TEST_DIR"/flow/unreachable_ignored.sk",

"; ModuleID = './test/sh/flow/unreachable_ignored.sk'\n" \
"source_filename = \"./test/sh/flow/unreachable_ignored.sk\"\n" \
"\n" \
"define i64 @.unreachable_ignored() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	f() {\n" \
		"unreachable\n" \
"	}\n",

TEST_DIR"/flow/unreachable_in_func.sk",

"; ModuleID = './test/sh/flow/unreachable_in_func.sk'\n" \
"source_filename = \"./test/sh/flow/unreachable_in_func.sk\"\n" \
"\n" \
"define i64 @.unreachable_in_func() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private void @f() {\n" \
"entry:\n" \
"  unreachable\n" \
"}\n",

NULL
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

"; ModuleID = './test/sh/flow/while.sk'\n" \
"source_filename = \"./test/sh/flow/while.sk\"\n" \
"\n" \
"define i64 @.while() {\n" \
"entry:\n" \
"  br label %while_cond\n" \
"\n" \
"while_cond:                                       ; preds = %while_loop, %entry\n" \
"  br i1 true, label %while_loop, label %while_end\n" \
"\n" \
"while_loop:                                       ; preds = %while_cond\n" \
"  br label %while_cond\n" \
"\n" \
"while_end:                                        ; preds = %while_cond\n" \
"  br label %while_cond1\n" \
"\n" \
"while_cond1:                                      ; preds = %while_loop2, %while_end\n" \
"  br i1 true, label %while_loop2, label %while_end3\n" \
"\n" \
"while_loop2:                                      ; preds = %while_cond1\n" \
"  br label %while_cond1\n" \
"\n" \
"while_end3:                                       ; preds = %while_cond1\n" \
"  br label %while_cond4\n" \
"\n" \
"while_cond4:                                      ; preds = %while_loop5, %while_end3\n" \
"  %0 = call i1 @f()\n" \
"  br i1 %0, label %while_loop5, label %while_end6\n" \
"\n" \
"while_loop5:                                      ; preds = %while_cond4\n" \
"  br label %while_cond4\n" \
"\n" \
"while_end6:                                       ; preds = %while_cond4\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private i1 @f() {\n" \
"entry:\n" \
"  ret i1 true\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	external func() Int\n" \
"	\n" \
"	mut x := func()\n" \
"	\n" \
"	return x\n",

TEST_DIR"/function/assign_return_value.sk",

"; ModuleID = './test/sh/function/assign_return_value.sk'\n" \
"source_filename = \"./test/sh/function/assign_return_value.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.assign_return_value() {\n" \
"entry:\n" \
"  %0 = call i64 @func()\n" \
"  store i64 %0, i64* @x, align 4\n" \
"  %1 = load i64, i64* @x, align 4\n" \
"  ret i64 %1\n" \
"}\n" \
"\n" \
"declare i64 @func()\n",

NULL
);


pass |= e2e_wrapper(
"	external puts(s: Str)\n" \
"	\n" \
"	puts(\"hello world!\")\n",

TEST_DIR"/function/call_str.sk",

"; ModuleID = './test/sh/function/call_str.sk'\n" \
"source_filename = \"./test/sh/function/call_str.sk\"\n" \
"\n" \
"@0 = private unnamed_addr constant [13 x i8] c\"hello world!\\00\", align 1\n" \
"\n" \
"define i64 @.call_str() {\n" \
"entry:\n" \
"  call void @puts(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @0, i32 0, i32 0))\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare void @puts(i8*)\n",

NULL
);


pass |= e2e_wrapper(
"	f() {\n" \
		"# do nothing\n" \
"	}\n",

TEST_DIR"/function/declare_func.sk",

"; ModuleID = './test/sh/function/declare_func.sk'\n" \
"source_filename = \"./test/sh/function/declare_func.sk\"\n" \
"\n" \
"define i64 @.declare_func() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private void @f() {\n" \
"entry:\n" \
"  ret void\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	export f() {\n" \
		"# do nothing\n" \
"	}\n",

TEST_DIR"/function/exported.sk",

"; ModuleID = './test/sh/function/exported.sk'\n" \
"source_filename = \"./test/sh/function/exported.sk\"\n" \
"\n" \
"define i64 @.exported() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define void @f() {\n" \
"entry:\n" \
"  ret void\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	f(num: Int) {\n" \
		"# do nothing\n" \
"	}\n" \
"	\n" \
"	f(1 + 2)\n",

TEST_DIR"/function/expr_param.sk",

"; ModuleID = './test/sh/function/expr_param.sk'\n" \
"source_filename = \"./test/sh/function/expr_param.sk\"\n" \
"\n" \
"define i64 @.expr_param() {\n" \
"entry:\n" \
"  call void @f(i64 3)\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private void @f(i64 %0) {\n" \
"entry:\n" \
"  ret void\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	external x() Int\n" \
"	external y(num: Int) Int\n",
TEST_DIR"/function/external_return_type.sk",

"; ModuleID = './test/sh/function/external_return_type.sk'\n" \
"source_filename = \"./test/sh/function/external_return_type.sk\"\n" \
"\n" \
"define i64 @.external_return_type() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare i64 @x()\n" \
"\n" \
"declare i64 @y(i64)\n",

NULL
);


pass |= e2e_wrapper(
"	f() {\n" \
		"mut x := 1\n" \
"	}\n" \
"	\n" \
"	mut x := 0\n",

TEST_DIR"/function/func_with_scope.sk",

"; ModuleID = './test/sh/function/func_with_scope.sk'\n" \
"source_filename = \"./test/sh/function/func_with_scope.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.func_with_scope() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private void @f() {\n" \
"entry:\n" \
"  %x = alloca i64, align 8\n" \
"  store i64 1, i64* %x, align 4\n" \
"  ret void\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	f(a: Int, b: Float) {\n" \
		"# do nothing\n" \
"	}\n",

TEST_DIR"/function/multi_param_func_different_types.sk",

"; ModuleID = './test/sh/function/multi_param_func_different_types.sk'\n" \
"source_filename = \"./test/sh/function/multi_param_func_different_types.sk\"\n" \
"\n" \
"define i64 @.multi_param_func_different_types() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private void @f(i64 %0, double %1) {\n" \
"entry:\n" \
"  ret void\n" \
"}\n",

NULL
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

"; ModuleID = './test/sh/function/multi_param_func.sk'\n" \
"source_filename = \"./test/sh/function/multi_param_func.sk\"\n" \
"\n" \
"define i64 @.multi_param_func() {\n" \
"entry:\n" \
"  %0 = call i64 @f(i64 1, i64 2)\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private i64 @f(i64 %0, i64 %1) {\n" \
"entry:\n" \
"  %2 = add nsw i64 %0, %1\n" \
"  ret i64 %2\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	external a()\n" \
"	external b()\n" \
"	\n" \
"	a()\n",

TEST_DIR"/function/multiple_externals.sk",

"; ModuleID = './test/sh/function/multiple_externals.sk'\n" \
"source_filename = \"./test/sh/function/multiple_externals.sk\"\n" \
"\n" \
"define i64 @.multiple_externals() {\n" \
"entry:\n" \
"  call void @a()\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare void @a()\n" \
"\n" \
"declare void @b()\n",

NULL
);


pass |= e2e_wrapper(
"	f() {\n" \
		"if true {\n" \
			"# do nothing\n" \
		"}\n" \
"	}\n",

TEST_DIR"/function/nested_if.sk",

"; ModuleID = './test/sh/function/nested_if.sk'\n" \
"source_filename = \"./test/sh/function/nested_if.sk\"\n" \
"\n" \
"define i64 @.nested_if() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private void @f() {\n" \
"entry:\n" \
"  br i1 true, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  br label %end\n" \
"\n" \
"end:                                              ; preds = %entry, %if_true\n" \
"  ret void\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	external x()\n" \
"	\n" \
"	x()\n",

TEST_DIR"/function/no_args.sk",

"; ModuleID = './test/sh/function/no_args.sk'\n" \
"source_filename = \"./test/sh/function/no_args.sk\"\n" \
"\n" \
"define i64 @.no_args() {\n" \
"entry:\n" \
"  call void @x()\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare void @x()\n",

NULL
);


pass |= e2e_wrapper(
"	external x()\n" \
"	\n" \
"	x()\n" \
"	\n" \
"	return 1\n",

TEST_DIR"/function/nodes_after_call_not_ignored.sk",

"; ModuleID = './test/sh/function/nodes_after_call_not_ignored.sk'\n" \
"source_filename = \"./test/sh/function/nodes_after_call_not_ignored.sk\"\n" \
"\n" \
"define i64 @.nodes_after_call_not_ignored() {\n" \
"entry:\n" \
"  call void @x()\n" \
"  ret i64 1\n" \
"}\n" \
"\n" \
"declare void @x()\n",

NULL
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

"; ModuleID = './test/sh/function/non_int_return.sk'\n" \
"source_filename = \"./test/sh/function/non_int_return.sk\"\n" \
"\n" \
"define i64 @.non_int_return() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private double @func() {\n" \
"entry:\n" \
"  %ret = alloca double, align 8\n" \
"  store double 3.140000e+00, double* %ret, align 8\n" \
"  %0 = load double, double* %ret, align 8\n" \
"  ret double %0\n" \
"}\n" \
"\n" \
"define private double @func2() {\n" \
"entry:\n" \
"  ret double 3.140000e+00\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	external x(a: Int)\n" \
"	\n" \
"	x(1)\n",

TEST_DIR"/function/single_param.sk",

"; ModuleID = './test/sh/function/single_param.sk'\n" \
"source_filename = \"./test/sh/function/single_param.sk\"\n" \
"\n" \
"define i64 @.single_param() {\n" \
"entry:\n" \
"  call void @x(i64 1)\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare void @x(i64)\n",

NULL
);


pass |= e2e_wrapper(
"	f() {\n" \
		"return\n" \
"	}\n",

TEST_DIR"/function/void_explicit_ret.sk",

"; ModuleID = './test/sh/function/void_explicit_ret.sk'\n" \
"source_filename = \"./test/sh/function/void_explicit_ret.sk\"\n" \
"\n" \
"define i64 @.void_explicit_ret() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private void @f() {\n" \
"entry:\n" \
"  ret void\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	# comment\n" \
"	x := 0\n",

TEST_DIR"/misc/comment_then_code.sk",

"; ModuleID = './test/sh/misc/comment_then_code.sk'\n" \
"source_filename = \"./test/sh/misc/comment_then_code.sk\"\n" \
"\n" \
"define i64 @.comment_then_code() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
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

"; ModuleID = './test/sh/misc/const_inline.sk'\n" \
"source_filename = \"./test/sh/misc/const_inline.sk\"\n" \
"\n" \
"define i64 @.const_inline() {\n" \
"entry:\n" \
"  ret i64 3\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := \"\\n\"\n" \
"	mut y := '\\n'\n",

TEST_DIR"/misc/escape_sequences.sk",

"; ModuleID = './test/sh/misc/escape_sequences.sk'\n" \
"source_filename = \"./test/sh/misc/escape_sequences.sk\"\n" \
"\n" \
"@0 = private unnamed_addr constant [2 x i8] c\"\\0A\\00\", align 1\n" \
"@x = private global i8* getelementptr inbounds ([2 x i8], [2 x i8]* @0, i32 0, i32 0)\n" \
"@y = private global i32 10\n" \
"\n" \
"define i64 @.escape_sequences() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	emoji := \"😀\"\n",

TEST_DIR"/misc/locale.sk",

"; ModuleID = './test/sh/misc/locale.sk'\n" \
"source_filename = \"./test/sh/misc/locale.sk\"\n" \
"\n" \
"@0 = private unnamed_addr constant [5 x i8] c\"\\F0\\9F\\98\\80\\00\", align 1\n" \
"\n" \
"define i64 @.locale() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	return 0",

TEST_DIR"/misc/no_eol_works.sk",

"; ModuleID = './test/sh/misc/no_eol_works.sk'\n" \
"source_filename = \"./test/sh/misc/no_eol_works.sk\"\n" \
"\n" \
"define i64 @.no_eol_works() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	if true {\n" \
		"noop\n" \
"	}\n" \
"	\n" \
"	noop\n",

TEST_DIR"/misc/noop.sk",

"; ModuleID = './test/sh/misc/noop.sk'\n" \
"source_filename = \"./test/sh/misc/noop.sk\"\n" \
"\n" \
"define i64 @.noop() {\n" \
"entry:\n" \
"  br i1 true, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  br label %end\n" \
"\n" \
"end:                                              ; preds = %entry, %if_true\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	if false {\n" \
		"x := Int\n" \
"	}\n",

TEST_DIR"/misc/not_top_lvl_alias.sk",

"; ModuleID = './test/sh/misc/not_top_lvl_alias.sk'\n" \
"source_filename = \"./test/sh/misc/not_top_lvl_alias.sk\"\n" \
"\n" \
"define i64 @.not_top_lvl_alias() {\n" \
"entry:\n" \
"  br i1 false, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  br label %end\n" \
"\n" \
"end:                                              ; preds = %entry, %if_true\n" \
"  ret i64 0\n" \
"}\n",

NULL
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

"; ModuleID = './test/sh/misc/preserve_nested_scopes.sk'\n" \
"source_filename = \"./test/sh/misc/preserve_nested_scopes.sk\"\n" \
"\n" \
"define i64 @.preserve_nested_scopes() {\n" \
"entry:\n" \
"  br i1 true, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  br i1 true, label %if_true2, label %end1\n" \
"\n" \
"end:                                              ; preds = %entry, %end1\n" \
"  ret i64 0\n" \
"\n" \
"if_true2:                                         ; preds = %if_true\n" \
"  br label %end1\n" \
"\n" \
"end1:                                             ; preds = %if_true, %if_true2\n" \
"  br label %end\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	f(a: Int, b: Int, c: Int) { noop }\n",

TEST_DIR"/misc/triple_param_no_overflow.sk",

"; ModuleID = './test/sh/misc/triple_param_no_overflow.sk'\n" \
"source_filename = \"./test/sh/misc/triple_param_no_overflow.sk\"\n" \
"\n" \
"define i64 @.triple_param_no_overflow() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private void @f(i64 %0, i64 %1, i64 %2) {\n" \
"entry:\n" \
"  ret void\n" \
"}\n",

NULL
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

"; ModuleID = './test/sh/misc/type_alias.sk'\n" \
"source_filename = \"./test/sh/misc/type_alias.sk\"\n" \
"\n" \
"@x = private global i64 1234\n" \
"\n" \
"define i64 @.type_alias() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private i64 @f(i64 %0) {\n" \
"entry:\n" \
"  ret i64 %0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := 2.0 ^ 3.0\n",

TEST_DIR"/oper/float_pow.sk",

"; ModuleID = './test/sh/oper/float_pow.sk'\n" \
"source_filename = \"./test/sh/oper/float_pow.sk\"\n" \
"\n" \
"@x = private global double 0.000000e+00\n" \
"\n" \
"define i64 @.float_pow() {\n" \
"entry:\n" \
"  %0 = call double @_float_pow(double 2.000000e+00, double 3.000000e+00)\n" \
"  store double %0, double* @x, align 8\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare double @_float_pow(double, double)\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := 10 ^ 3\n",

TEST_DIR"/oper/int_pow.sk",

"; ModuleID = './test/sh/oper/int_pow.sk'\n" \
"source_filename = \"./test/sh/oper/int_pow.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.int_pow() {\n" \
"entry:\n" \
"  %0 = call i64 @_int_pow(i64 10, i64 3)\n" \
"  store i64 %0, i64* @x, align 4\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare i64 @_int_pow(i64, i64)\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := \"x\" is \"x\"\n",

TEST_DIR"/oper/is_cmp_str.sk",

"; ModuleID = './test/sh/oper/is_cmp_str.sk'\n" \
"source_filename = \"./test/sh/oper/is_cmp_str.sk\"\n" \
"\n" \
"@0 = private unnamed_addr constant [2 x i8] c\"x\\00\", align 1\n" \
"@1 = private unnamed_addr constant [2 x i8] c\"x\\00\", align 1\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.is_cmp_str() {\n" \
"entry:\n" \
"  %0 = call i1 @_strcmp(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @1, i32 0, i32 0))\n" \
"  store i1 %0, i1* @x, align 1\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare i1 @_strcmp(i8*, i8*)\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := 1\n" \
"	mut y := x + 2\n" \
"	mut z := x < 1\n",

TEST_DIR"/oper/lhs_with_var.sk",

"; ModuleID = './test/sh/oper/lhs_with_var.sk'\n" \
"source_filename = \"./test/sh/oper/lhs_with_var.sk\"\n" \
"\n" \
"@x = private global i64 1\n" \
"@y = private global i64 0\n" \
"@z = private global i1 false\n" \
"\n" \
"define i64 @.lhs_with_var() {\n" \
"entry:\n" \
"  %0 = load i64, i64* @x, align 4\n" \
"  %1 = add nsw i64 %0, 2\n" \
"  store i64 %1, i64* @y, align 4\n" \
"  %2 = load i64, i64* @x, align 4\n" \
"  %3 = icmp slt i64 %2, 1\n" \
"  store i1 %3, i1* @z, align 1\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := (1 + 2)\n" \
"	\n" \
"	mut y := (((1 + 2)))\n",

TEST_DIR"/oper/paren_expr.sk",

"; ModuleID = './test/sh/oper/paren_expr.sk'\n" \
"source_filename = \"./test/sh/oper/paren_expr.sk\"\n" \
"\n" \
"@x = private global i64 3\n" \
"@y = private global i64 3\n" \
"\n" \
"define i64 @.paren_expr() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := 1 + (2 + 3)\n",

TEST_DIR"/oper/paren_rhs_expr.sk",

"; ModuleID = './test/sh/oper/paren_rhs_expr.sk'\n" \
"source_filename = \"./test/sh/oper/paren_rhs_expr.sk\"\n" \
"\n" \
"@x = private global i64 6\n" \
"\n" \
"define i64 @.paren_rhs_expr() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut x := 1\n" \
"	\n" \
"	mut z := 2 + x\n",

TEST_DIR"/oper/rhs_with_var.sk",

"; ModuleID = './test/sh/oper/rhs_with_var.sk'\n" \
"source_filename = \"./test/sh/oper/rhs_with_var.sk\"\n" \
"\n" \
"@x = private global i64 1\n" \
"@z = private global i64 0\n" \
"\n" \
"define i64 @.rhs_with_var() {\n" \
"entry:\n" \
"  %0 = load i64, i64* @x, align 4\n" \
"  %1 = add nsw i64 2, %0\n" \
"  store i64 %1, i64* @z, align 4\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"	mut w := - 1.0\n" \
"	mut x := not false\n" \
"	mut y := - 1\n" \
"	mut z := - y\n",

TEST_DIR"/oper/unary_expr.sk",

"; ModuleID = './test/sh/oper/unary_expr.sk'\n" \
"source_filename = \"./test/sh/oper/unary_expr.sk\"\n" \
"\n" \
"@w = private global double -1.000000e+00\n" \
"@x = private global i1 true\n" \
"@y = private global i64 -1\n" \
"@z = private global i64 0\n" \
"\n" \
"define i64 @.unary_expr() {\n" \
"entry:\n" \
"  %0 = load i64, i64* @y, align 4\n" \
"  %1 = sub nsw i64 0, %0\n" \
"  store i64 %1, i64* @z, align 4\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


/* TODO
pass |= e2e_wrapper(
"a(x: Int) { noop }\n" \
"b(x: Int) { noop }\n" \
"external c(x: Int)\n" \
"external d(x: Int)\n",

TEST_DIR"/func/per_scope_params.sk",

"; ModuleID = 'tmp.sk'\n" \
"source_filename = \"tmp.sk\"\n" \
"\n" \
"define i64 @.tmp() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private void @a(i64 %0) {\n" \
"entry:\n" \
"  ret void\n" \
"}\n" \
"\n" \
"define private void @b(i64 %0) {\n" \
"entry:\n" \
"  ret void\n" \
"}\n" \
"\n" \
"declare void @c(i64)\n" \
"\n" \
"declare void @d(i64)\n",

NULL
);
*/


pass |= e2e_wrapper(
"a := Int\n" \
"b := a\n" \
"c: b = 1\n",

TEST_DIR"/misc/alias_type_alias.sk",

"; ModuleID = './test/sh/misc/alias_type_alias.sk'\n" \
"source_filename = \"./test/sh/misc/alias_type_alias.sk\"\n" \
"\n" \
"define i64 @.alias_type_alias() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"while true {\n" \
"	break\n"
"}",

TEST_DIR"/flow/break.sk",

"; ModuleID = './test/sh/flow/break.sk'\n" \
"source_filename = \"./test/sh/flow/break.sk\"\n" \
"\n" \
"define i64 @.break() {\n" \
"entry:\n" \
"  br label %while_cond\n" \
"\n" \
"while_cond:                                       ; preds = %while_loop, %entry\n" \
"  br i1 true, label %while_loop, label %while_end\n" \
"\n" \
"while_loop:                                       ; preds = %while_cond\n" \
"  br label %while_end\n" \
"  br label %while_cond\n" \
"\n" \
"while_end:                                        ; preds = %while_loop, %while_cond\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


pass |= e2e_wrapper(
"while true {\n" \
"  continue\n"
"}",

TEST_DIR"/flow/continue.sk",

"; ModuleID = './test/sh/flow/continue.sk'\n" \
"source_filename = \"./test/sh/flow/continue.sk\"\n" \
"\n" \
"define i64 @.continue() {\n" \
"entry:\n" \
"  br label %while_cond\n" \
"\n" \
"while_cond:                                       ; preds = %while_loop, %while_loop, %entry\n" \
"  br i1 true, label %while_loop, label %while_end\n" \
"\n" \
"while_loop:                                       ; preds = %while_cond\n" \
"  br label %while_cond\n" \
"  br label %while_cond\n" \
"\n" \
"while_end:                                        ; preds = %while_cond\n" \
"  ret i64 0\n" \
"}\n",

NULL
);


// error tests

pass |= e2e_wrapper(
"x: Int =\n",
TEST_DIR"/error/declare/assign_missing_rhs.sk",

NULL,

"./test/sh/error/declare/assign_missing_rhs.sk: Compilation error: line 1 column 9: expected expression in assignment variable\n"
);


pass |= e2e_wrapper(
"x := y\n",
TEST_DIR"/error/declare/assign_unknown_var.sk",

NULL,

"./test/sh/error/declare/assign_unknown_var.sk: Compilation error: line 1 column 6: variable \"y\" not found\n"
);


pass |= e2e_wrapper(
"x :=\n",
TEST_DIR"/error/declare/auto_assign_missing_rhs.sk",

NULL,

"./test/sh/error/declare/auto_assign_missing_rhs.sk: Compilation error: line 1 column 5: expected expression in assignment variable\n"
);


pass |= e2e_wrapper(
"mut x := 0\n" \
"x =\n",
TEST_DIR"/error/declare/reassign_missing_rhs.sk",

NULL,

"./test/sh/error/declare/reassign_missing_rhs.sk: Compilation error: line 2 column 4: expected expression in assignment variable\n"
);


pass |= e2e_wrapper(
"if true {}\n",
TEST_DIR"/error/flow/empty_block.sk",

NULL,

"./test/sh/error/flow/empty_block.sk: Compilation error: line 1 column 10: code block cannot be empty\n"
);


pass |= e2e_wrapper(
"if true\n",
TEST_DIR"/error/flow/if_missing_block.sk",

NULL,

"./test/sh/error/flow/if_missing_block.sk: Compilation error: line 1 column 1: if statement must be followed by code block\n"
);


pass |= e2e_wrapper(
"if true {\n" \
"	x 1\n" \
"}\n" \
"\n" \
"return 2\n",
TEST_DIR"/error/flow/invalid_return_in_if.sk",

NULL,

"./test/sh/error/flow/invalid_return_in_if.sk: Warning: line 1 column 4: condition is always true\n" \
"./test/sh/error/flow/invalid_return_in_if.sk: Compilation error: line 2 column 2: expression cannot be used on its own\n"
);


pass |= e2e_wrapper(
"if # this will fail\n",
TEST_DIR"/error/flow/missing_expr.sk",

NULL,

"./test/sh/error/flow/missing_expr.sk: Compilation error: line 1 column 4: invalid expression near \"# this will fail\"\n"
);


pass |= e2e_wrapper(
"return # wont work\n",
TEST_DIR"/error/flow/return_missing_expr.sk",

NULL,

"./test/sh/error/flow/return_missing_expr.sk: Compilation error: line 1 column 1: expected expression in return\n"
);


pass |= e2e_wrapper(
"return x\n",
TEST_DIR"/error/flow/return_non_existent_var.sk",

NULL,

"./test/sh/error/flow/return_non_existent_var.sk: Compilation error: line 1 column 8: variable \"x\" not found\n"
);


pass |= e2e_wrapper(
"# TODO: fix incorrect error (should be cannot assign type)\n" \
"\n" \
"func() Int {\n" \
"	return 1\n" \
"}\n" \
"\n" \
"x: Float = func()\n",
TEST_DIR"/error/func/assign_bad_type.sk",

NULL,

"./test/sh/error/func/assign_bad_type.sk: Compilation error: line 7 column 12: expected type \"Float\", got \"Int\"\n"
);


pass |= e2e_wrapper(
"f() {\n" \
"	# do nothing\n" \
"}\n" \
"\n" \
"x := f()\n",
TEST_DIR"/error/func/assign_void_ret.sk",

NULL,

"./test/sh/error/func/assign_void_ret.sk: Compilation error: line 5 column 6: function returning type void cannot be assigned to variable \"x\"\n"
);


pass |= e2e_wrapper(
"f(b: Bool) {\n" \
"	# do nothing\n" \
"}\n" \
"\n" \
"f(1 + 2)\n",
TEST_DIR"/error/func/bad_expr_param.sk",

NULL,

"./test/sh/error/func/bad_expr_param.sk: Compilation error: line 5 column 3: expected param of type \"Bool\", got \"Int\"\n"
);


pass |= e2e_wrapper(
"external x(num: Int)\n" \
"\n" \
"x(false)\n",
TEST_DIR"/error/func/bad_param_type.sk",

NULL,

"./test/sh/error/func/bad_param_type.sk: Compilation error: line 3 column 3: expected param of type \"Int\", got \"Bool\"\n"
);


pass |= e2e_wrapper(
"x() { noop }\n" \
"x := Int\n",
TEST_DIR"/error/func/func_as_alias.sk",

NULL,

"./test/sh/error/func/func_as_alias.sk: Compilation error: line 2 column 1: cannot redeclare function \"x\" as type alias\n"
);


pass |= e2e_wrapper(
"x() { return }\n" \
"\n" \
"x := 1\n",
TEST_DIR"/error/func/func_as_var.sk",

NULL,

"./test/sh/error/func/func_as_var.sk: Compilation error: line 3 column 1: cannot redeclare function \"x\" as variable\n"
);


pass |= e2e_wrapper(
"f(x: Int) {\n" \
"	# do nothing\n" \
"}\n" \
"\n" \
"f(1, 2, 3)\n",
TEST_DIR"/error/func/invalid_num_params.sk",

NULL,

"./test/sh/error/func/invalid_num_params.sk: Compilation error: line 5 column 1: invalid number of parameters\n"
);


pass |= e2e_wrapper(
"return f(",
TEST_DIR"/error/func/missing_closing_paren.sk",

NULL,

"./test/sh/error/func/missing_closing_paren.sk: Compilation error: line 1 column 8: function call missing closing parenthesis\n"
);


pass |= e2e_wrapper(
"external f(a: Int, b: Int)\n" \
"\n" \
"f(1 2)\n",
TEST_DIR"/error/func/multi_param_call_missing_comma.sk",

NULL,

"./test/sh/error/func/multi_param_call_missing_comma.sk: Compilation error: line 3 column 5: expected comma\n"
);


pass |= e2e_wrapper(
"f(x: Int y: Int) {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/func/multi_param_missing_comma.sk",

NULL,

"./test/sh/error/func/multi_param_missing_comma.sk: Compilation error: line 1 column 10: expected comma\n"
);


pass |= e2e_wrapper( //
"external f(x: Int)\n" \
"\n" \
"x := false\n" \
"\n" \
"f(x)\n",
TEST_DIR"/error/func/param_bad_type.sk",

NULL,

"./test/sh/error/func/param_bad_type.sk: Compilation error: line 5 column 3: expected param of type \"Int\", got \"Bool\"\n"
);


pass |= e2e_wrapper(
"external f(x: Int)\n" \
"\n" \
"f(fail)\n",
TEST_DIR"/error/func/param_unknown_var.sk",

NULL,

"./test/sh/error/func/param_unknown_var.sk: Compilation error: line 3 column 3: variable \"fail\" not found\n"
);


pass |= e2e_wrapper( //
"f(x: does_not_exist) {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/func/proto_param_bad_type.sk",

NULL,

"./test/sh/error/func/proto_param_bad_type.sk: Compilation error: type \"does_not_exist\" could not be found\n"
);


pass |= e2e_wrapper( //
"x := 0\n" \
"\n" \
"f(x: Int) Int {\n" \
"	return x\n" \
"}\n",
TEST_DIR"/error/func/shadowed_param.sk",

NULL,

"./test/sh/error/func/shadowed_param.sk: Compilation error: line 3 column 3: variable \"x\" shadows existing variable\n"
);


pass |= e2e_wrapper( //
"if true {\n" \
"	return 1\n" \
"} {\n" \
"	return 2\n" \
"}\n" \
"\n" \
"return 0\n",
TEST_DIR"/error/misc/duplicated_code_block.sk",

NULL,

"./test/sh/error/misc/duplicated_code_block.sk: Compilation error: line 3 column 3: unexpected code block\n"
);


pass |= e2e_wrapper(
"if true { return 1 }}\n",
TEST_DIR"/error/misc/extra_close_bracket.sk",

NULL,

"./test/sh/error/misc/extra_close_bracket.sk: Compilation error: line 1 column 21: missing opening bracket\n"
);


pass |= e2e_wrapper(
"x!@$ := \"this will fail\"\n",
TEST_DIR"/error/misc/invalid_identifier.sk",

NULL,

"./test/sh/error/misc/invalid_identifier.sk: Compilation error: line 1 column 1: unexpected token: \"x!@$\"\n"
);


pass |= e2e_wrapper(
"x := (_)\n",
TEST_DIR"/error/misc/invalid_paren_expr.sk",

NULL,

"./test/sh/error/misc/invalid_paren_expr.sk: Compilation error: line 1 column 7: invalid expression near \"_\"\n"
);


pass |= e2e_wrapper(
"if true {{\n" \
"	return 1\n" \
"}\n" \
"\n" \
"return 0\n",
TEST_DIR"/error/misc/mismatched_bracket.sk",

NULL,

"./test/sh/error/misc/mismatched_bracket.sk: Compilation error: Reached EOF, expected closing bracket\n"
);


pass |= e2e_wrapper(
"if false {\n" \
"	export func() {\n" \
		"# this will fail\n" \
"	}\n" \
"}\n" \
"\n" \
"func()\n",
TEST_DIR"/error/misc/no_nested_func.sk",

NULL,

"./test/sh/error/misc/no_nested_func.sk: Warning: line 1 column 4: condition is always false\n" \
"./test/sh/error/misc/no_nested_func.sk: Compilation error: line 2 column 9: cannot declare nested function \"func\" as external or exported\n"
);


pass |= e2e_wrapper(
"external x() x()\n",
TEST_DIR"/error/misc/no_newline_external.sk",

NULL,

"./test/sh/error/misc/no_newline_external.sk: Compilation error: line 1 column 1: unexpected token: \"external\"\n"
);


pass |= e2e_wrapper(
"{ noop }\n",
TEST_DIR"/error/misc/only_brackets.sk",

NULL,

"./test/sh/error/misc/only_brackets.sk: Compilation error: line 1 column 3: unexpected code block\n"
);


pass |= e2e_wrapper(
"x := (1\n",
TEST_DIR"/error/misc/paren_expr_missing_paren.sk",

NULL,

"./test/sh/error/misc/paren_expr_missing_paren.sk: Compilation error: line 1 column 8: missing closing parenthesis\n"
);


pass |= e2e_wrapper(
"}\n",
TEST_DIR"/error/misc/stray_close_bracket.sk",

NULL,

"./test/sh/error/misc/stray_close_bracket.sk: Compilation error: line 1 column 1: missing opening bracket\n"
);


pass |= e2e_wrapper(
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

NULL,

"./test/sh/error/misc/token_iter_keep_loc.sk: Compilation error: line 12 column 1: expression cannot be used on its own\n"
);


pass |= e2e_wrapper(
"# do nothing\n" \
"\n" \
"{\n" \
"	# not allowed\n" \
"}\n",
TEST_DIR"/error/misc/unexpected_code_block.sk",

NULL,

"./test/sh/error/misc/unexpected_code_block.sk: Compilation error: line 4 column 2: unexpected code block\n"
);


pass |= e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 + \"fail\"\n",
TEST_DIR"/error/oper/add_mismatched_consts.sk",

NULL,

"./test/sh/error/oper/add_mismatched_consts.sk: Compilation error: line 3 column 9: expected type \"Int\", got \"Str\"\n"
);


pass |= e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 / \"fail\"\n",
TEST_DIR"/error/oper/div_mismatched_consts.sk",

NULL,

"./test/sh/error/oper/div_mismatched_consts.sk: Compilation error: line 3 column 9: expected type \"Int\", got \"Str\"\n"
);


pass |= e2e_wrapper(
"x := 1 +\n",
TEST_DIR"/error/oper/missing_rhs.sk",

NULL,

"./test/sh/error/oper/missing_rhs.sk: Compilation error: line 1 column 8: expected expression after \"+\"\n"
);


pass |= e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 * \"fail\"\n",
TEST_DIR"/error/oper/mult_mismatched_consts.sk",

NULL,

"./test/sh/error/oper/mult_mismatched_consts.sk: Compilation error: line 3 column 9: expected type \"Int\", got \"Str\"\n"
);


pass |= e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 - \"fail\"\n",
TEST_DIR"/error/oper/sub_mismatched_consts.sk",

NULL,

"./test/sh/error/oper/sub_mismatched_consts.sk: Compilation error: line 3 column 9: expected type \"Int\", got \"Str\"\n"
);


pass |= e2e_wrapper(
"mut x = 0",
TEST_DIR"/error/misc/invalid_mut.sk",

NULL,

"./test/sh/error/misc/invalid_mut.sk: Compilation error: line 1 column 1: unexpected token: \"mut\"\n"
);


pass |= e2e_wrapper(
"f() { noop }\nx := 1 + f()",
TEST_DIR"/error/function/void_func_expr.sk",

NULL,

"./test/sh/error/function/void_func_expr.sk: Compilation error: line 2 column 10: expected type \"Int\", got \"Void\"\n"
);


pass |= e2e_wrapper(
"x := not not true",
TEST_DIR"/error/oper/disallow_double_unary_oper.sk",

NULL,

"./test/sh/error/oper/disallow_double_unary_oper.sk: Compilation error: line 1 column 10: cannot use unary operator twice\n"
);


pass |= e2e_wrapper(
"x := true not true",
TEST_DIR"/error/oper/check_unary_oper.sk",

NULL,

"./test/sh/error/oper/check_unary_oper.sk: Compilation error: line 1 column 11: unexpected unary operator\n"
);

	return pass;
}


static int e2e_wrapper(const char *code, const char *mock_file, const char *llvm_expected, const char *err_expected) {
	BUILD_DATA.quiet = !err_expected;
	BUILD_DATA.filename = mock_file;
	BUILD_DATA.preprocess = true;
	BUILD_DATA.out_file = llvm_expected ?
		mock_file_to_output_file(mock_file, ".ll") :
		NULL;

	char *tmp = strdup(code);
	bool err = run_pipeline(mock_file, tmp);

	if (err_expected) {
		if (!llvm_expected) err = !err;
		err |= compare_errors(err_expected);
	}
	if (llvm_expected) {
		err |= compare_compiler_output(BUILD_DATA.out_file, llvm_expected);
		free(BUILD_DATA.out_file);
	}

	free_errors();

	if (err) {
		// TODO(dosisod) give better error on why it failed
		printf("%s " COLOR_BOLD COLOR_RED_FG "FAIL" COLOR_RESET "\n", mock_file);
	}
	return err;
}

static bool compare_compiler_output(const char *filename, const char *expected) {
	errno = 0;
	FILE *f = fopen(filename, "re");

	if (errno) {
		perror("fopen");
		return true;
	}

	while (*expected) {
		const int c = fgetc(f);
		if (c == EOF || c != *expected) {
			fclose(f);
			return true;
		}

		expected++;
	}

	fclose(f);
	return false;
}

static char *mock_file_to_output_file(const char *filename, const char *extension) {
	const size_t len = strlen(filename);
	const size_t ext_len = strlen(extension);

	char *output_file = malloc(len + ext_len + 2);
	char *last_slash = strrchr(filename, '/');
	strncpy(output_file, filename, (size_t)(last_slash - filename) + 1);
	output_file[last_slash - filename + 1] = '.';
	strncpy(
		output_file + (last_slash - filename) + 2,
		last_slash + 1,
		len - (long unsigned)(last_slash - filename)
	);
	strncpy(output_file + len + 1, extension, ext_len + 1);

	return output_file;
}
