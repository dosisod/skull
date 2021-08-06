#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/codegen/entry.h"
#include "skull/common/color.h"
#include "skull/common/local.h"

bool SKULL_TESTING = 0;
#define TEST_DIR "./test/sh"

static int e2e_wrapper(const char *, const char *, bool, const char *);
static char *mock_file_to_output_file(const char *, const char *);
static bool compare_compiler_output(const char *, const char *);

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

false,

"; ModuleID = './test/sh/declare/basic_types.sk'\n" \
"source_filename = \"./test/sh/declare/basic_types.sk\"\n" \
"\n" \
"@bool = private global i1 false\n" \
"@int = private global i64 0\n" \
"@float = private global double 0.000000e+00\n" \
"@0 = private unnamed_addr constant [12 x i8] c\"hello world\\00\", align 1\n" \
"@str = private global i8* null\n" \
"@rune = private global i32 0\n" \
"\n" \
"define i64 @.basic_types() {\n" \
"entry:\n" \
"  store i1 true, i1* @bool\n" \
"  store i64 1234, i64* @int\n" \
"  store double 3.140000e+00, double* @float\n" \
"  store i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0), i8** @str\n" \
"  store i32 120, i32* @rune\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := true and true\n",

TEST_DIR"/declare/bool_expr/and.sk",

false,

"; ModuleID = './test/sh/declare/bool_expr/and.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/and.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.and() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 1 >= 1\n" \
"	mut y := 1.0 >= 1.0\n",

TEST_DIR"/declare/bool_expr/gtr_than_eq.sk",

false,

"; ModuleID = './test/sh/declare/bool_expr/gtr_than_eq.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/gtr_than_eq.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"@y = private global i1 false\n" \
"\n" \
"define i64 @.gtr_than_eq() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  store i1 true, i1* @y\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 2 > 1\n" \
"	mut y := 2.0 > 1.0\n",

TEST_DIR"/declare/bool_expr/gtr_than.sk",

false,

"; ModuleID = './test/sh/declare/bool_expr/gtr_than.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/gtr_than.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"@y = private global i1 false\n" \
"\n" \
"define i64 @.gtr_than() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  store i1 true, i1* @y\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 1 isnt 2\n",

TEST_DIR"/declare/bool_expr/isnt.sk",

false,

"; ModuleID = './test/sh/declare/bool_expr/isnt.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/isnt.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.isnt() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 1 is 1\n" \
"	mut y := 1.0 is 1.0\n" \
"	mut z := 'x' is 'x'\n" \
"	mut q := true is true\n",

TEST_DIR"/declare/bool_expr/is.sk",

false,

"; ModuleID = './test/sh/declare/bool_expr/is.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/is.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"@y = private global i1 false\n" \
"@z = private global i1 false\n" \
"@q = private global i1 false\n" \
"\n" \
"define i64 @.is() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  store i1 true, i1* @y\n" \
"  store i1 true, i1* @z\n" \
"  store i1 true, i1* @q\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 1 <= 1\n" \
"	mut y := 1.0 <= 1.0\n",

TEST_DIR"/declare/bool_expr/less_than_eq.sk",

false,

"; ModuleID = './test/sh/declare/bool_expr/less_than_eq.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/less_than_eq.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"@y = private global i1 false\n" \
"\n" \
"define i64 @.less_than_eq() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  store i1 true, i1* @y\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 1 < 2\n" \
"	mut y := 1.0 < 2.0\n",

TEST_DIR"/declare/bool_expr/less_than.sk",

false,

"; ModuleID = './test/sh/declare/bool_expr/less_than.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/less_than.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"@y = private global i1 false\n" \
"\n" \
"define i64 @.less_than() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  store i1 true, i1* @y\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := not false\n",

TEST_DIR"/declare/bool_expr/not.sk",

false,

"; ModuleID = './test/sh/declare/bool_expr/not.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/not.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.not() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := false or true\n",

TEST_DIR"/declare/bool_expr/or.sk",

false,

"; ModuleID = './test/sh/declare/bool_expr/or.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/or.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.or() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	x := 1\n" \
"	\n" \
"	y := 1 is x\n",

TEST_DIR"/declare/bool_expr/var_rhs.sk",

false,

"; ModuleID = './test/sh/declare/bool_expr/var_rhs.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/var_rhs.sk\"\n" \
"\n" \
"define i64 @.var_rhs() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := false xor true\n",

TEST_DIR"/declare/bool_expr/xor.sk",

false,

"; ModuleID = './test/sh/declare/bool_expr/xor.sk'\n" \
"source_filename = \"./test/sh/declare/bool_expr/xor.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.xor() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	export x := 1\n",

TEST_DIR"/declare/export_var.sk",

false,

"; ModuleID = './test/sh/declare/export_var.sk'\n" \
"source_filename = \"./test/sh/declare/export_var.sk\"\n" \
"\n" \
"@x = global i64 1\n" \
"\n" \
"define i64 @.export_var() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 0\n" \
"	mut y := 0\n",

TEST_DIR"/declare/many_vars.sk",

false,

"; ModuleID = './test/sh/declare/many_vars.sk'\n" \
"source_filename = \"./test/sh/declare/many_vars.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"@y = private global i64 0\n" \
"\n" \
"define i64 @.many_vars() {\n" \
"entry:\n" \
"  store i64 0, i64* @x\n" \
"  store i64 0, i64* @y\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x: Int = 0\n",

TEST_DIR"/declare/mut_with_explicit_type.sk",

false,

"; ModuleID = './test/sh/declare/mut_with_explicit_type.sk'\n" \
"source_filename = \"./test/sh/declare/mut_with_explicit_type.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.mut_with_explicit_type() {\n" \
"entry:\n" \
"  store i64 0, i64* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := \"abc\"\n" \
"	x = \"defg\"\n",

TEST_DIR"/declare/reassign_str.sk",

false,

"; ModuleID = './test/sh/declare/reassign_str.sk'\n" \
"source_filename = \"./test/sh/declare/reassign_str.sk\"\n" \
"\n" \
"@0 = private unnamed_addr constant [4 x i8] c\"abc\\00\", align 1\n" \
"@x = private global i8* null\n" \
"@1 = private unnamed_addr constant [5 x i8] c\"defg\\00\", align 1\n" \
"\n" \
"define i64 @.reassign_str() {\n" \
"entry:\n" \
"  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8** @x\n" \
"  store i8* getelementptr inbounds ([5 x i8], [5 x i8]* @1, i32 0, i32 0), i8** @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 0\n" \
"	\n" \
"	x = 1\n",

TEST_DIR"/declare/reassign_var.sk",

false,

"; ModuleID = './test/sh/declare/reassign_var.sk'\n" \
"source_filename = \"./test/sh/declare/reassign_var.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.reassign_var() {\n" \
"entry:\n" \
"  store i64 0, i64* @x\n" \
"  store i64 1, i64* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	x := \"# \"\n",

TEST_DIR"/declare/str_with_comment.sk",

false,

"; ModuleID = './test/sh/declare/str_with_comment.sk'\n" \
"source_filename = \"./test/sh/declare/str_with_comment.sk\"\n" \
"\n" \
"@0 = private unnamed_addr constant [3 x i8] c\"# \\00\", align 1\n" \
"\n" \
"define i64 @.str_with_comment() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 0\n" \
"	mut z := x\n" \
"	\n" \
"	return z\n",

TEST_DIR"/declare/variable_auto_deduce.sk",

false,

"; ModuleID = './test/sh/declare/variable_auto_deduce.sk'\n" \
"source_filename = \"./test/sh/declare/variable_auto_deduce.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"@z = private global i64 0\n" \
"\n" \
"define i64 @.variable_auto_deduce() {\n" \
"entry:\n" \
"  store i64 0, i64* @x\n" \
"  %0 = load i64, i64* @x\n" \
"  store i64 %0, i64* @z\n" \
"  %1 = load i64, i64* @z\n" \
"  ret i64 %1\n" \
"}\n"
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

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	if true {\n" \
		"# do nothing\n" \
"	}\n" \
"	else {\n" \
		"# do nothing\n" \
"	}\n",

TEST_DIR"/flow/else.sk",

false,

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
"}\n"
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

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	if false {\n" \
	  "return 0\n" \
"	}\n" \
"	\n" \
"	return 1\n",

TEST_DIR"/flow/if/false.sk",

false,

"; ModuleID = './test/sh/flow/if/false.sk'\n" \
"source_filename = \"./test/sh/flow/if/false.sk\"\n" \
"\n" \
"define i64 @.false() {\n" \
"entry:\n" \
"  br i1 false, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  ret i64 0\n" \
"\n" \
"end:                                              ; preds = %entry\n" \
"  ret i64 1\n" \
"}\n"
);


pass |= e2e_wrapper(
"	if not true {\n" \
		"# do nothing\n" \
"	}\n",

TEST_DIR"/flow/if/not_with_const.sk",

false,

"; ModuleID = './test/sh/flow/if/not_with_const.sk'\n" \
"source_filename = \"./test/sh/flow/if/not_with_const.sk\"\n" \
"\n" \
"define i64 @.not_with_const() {\n" \
"entry:\n" \
"  br i1 false, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  br label %end\n" \
"\n" \
"end:                                              ; preds = %entry, %if_true\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := true\n" \
"	\n" \
"	if not x {\n" \
		"# do nothing\n" \
"	}\n",

TEST_DIR"/flow/if/not_with_var.sk",

false,

"; ModuleID = './test/sh/flow/if/not_with_var.sk'\n" \
"source_filename = \"./test/sh/flow/if/not_with_var.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.not_with_var() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  %0 = load i1, i1* @x\n" \
"  %1 = xor i1 %0, true\n" \
"  br i1 %1, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  br label %end\n" \
"\n" \
"end:                                              ; preds = %entry, %if_true\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	if true {\n" \
	  "return 0\n" \
"	}\n" \
"	\n" \
"	return 1\n",

TEST_DIR"/flow/if/true.sk",

false,

"; ModuleID = './test/sh/flow/if/true.sk'\n" \
"source_filename = \"./test/sh/flow/if/true.sk\"\n" \
"\n" \
"define i64 @.true() {\n" \
"entry:\n" \
"  br i1 true, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  ret i64 0\n" \
"\n" \
"end:                                              ; preds = %entry\n" \
"  ret i64 1\n" \
"}\n"
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

false,

"; ModuleID = './test/sh/flow/if/with_var_false.sk'\n" \
"source_filename = \"./test/sh/flow/if/with_var_false.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.with_var_false() {\n" \
"entry:\n" \
"  store i1 false, i1* @x\n" \
"  %0 = load i1, i1* @x\n" \
"  br i1 %0, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  ret i64 1\n" \
"\n" \
"end:                                              ; preds = %entry\n" \
"  ret i64 0\n" \
"}\n"
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

false,

"; ModuleID = './test/sh/flow/if/with_var_true.sk'\n" \
"source_filename = \"./test/sh/flow/if/with_var_true.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.with_var_true() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  %0 = load i1, i1* @x\n" \
"  br i1 %0, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  ret i64 1\n" \
"\n" \
"end:                                              ; preds = %entry\n" \
"  ret i64 0\n" \
"}\n"
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

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	return 0\n",

TEST_DIR"/flow/return_0.sk",

false,

"; ModuleID = './test/sh/flow/return_0.sk'\n" \
"source_filename = \"./test/sh/flow/return_0.sk\"\n" \
"\n" \
"define i64 @.return_0() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	return 1 + 2\n",

TEST_DIR"/flow/return_expr.sk",

false,

"; ModuleID = './test/sh/flow/return_expr.sk'\n" \
"source_filename = \"./test/sh/flow/return_expr.sk\"\n" \
"\n" \
"define i64 @.return_expr() {\n" \
"entry:\n" \
"  ret i64 3\n" \
"}\n"
);


pass |= e2e_wrapper(
"	external f() Int\n" \
"	\n" \
"	return f()\n",

TEST_DIR"/flow/return_func_value.sk",

false,

"; ModuleID = './test/sh/flow/return_func_value.sk'\n" \
"source_filename = \"./test/sh/flow/return_func_value.sk\"\n" \
"\n" \
"define i64 @.return_func_value() {\n" \
"entry:\n" \
"  %0 = call i64 @f()\n" \
"  ret i64 %0\n" \
"}\n" \
"\n" \
"declare i64 @f()\n"
);


pass |= e2e_wrapper(
"	mut x := 1\n" \
"	\n" \
"	return x\n",

TEST_DIR"/flow/return_var.sk",

false,

"; ModuleID = './test/sh/flow/return_var.sk'\n" \
"source_filename = \"./test/sh/flow/return_var.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.return_var() {\n" \
"entry:\n" \
"  store i64 1, i64* @x\n" \
"  %0 = load i64, i64* @x\n" \
"  ret i64 %0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	if true {\n" \
		"mut x := 1\n" \
"	}\n" \
"	\n" \
"	return 0\n",

TEST_DIR"/flow/set_var_in_if.sk",

false,

"; ModuleID = './test/sh/flow/set_var_in_if.sk'\n" \
"source_filename = \"./test/sh/flow/set_var_in_if.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.set_var_in_if() {\n" \
"entry:\n" \
"  br i1 true, label %if_true, label %end\n" \
"\n" \
"if_true:                                          ; preds = %entry\n" \
"  store i64 1, i64* @x\n" \
"  br label %end\n" \
"\n" \
"end:                                              ; preds = %entry, %if_true\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	return 0\n" \
"	unreachable\n" \
"	# some comment\n" \
"	noop\n",

TEST_DIR"/flow/unreachable_ignored.sk",

false,

"; ModuleID = './test/sh/flow/unreachable_ignored.sk'\n" \
"source_filename = \"./test/sh/flow/unreachable_ignored.sk\"\n" \
"\n" \
"define i64 @.unreachable_ignored() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"  unreachable\n" \
"}\n"
);


pass |= e2e_wrapper(
"	f() {\n" \
		"unreachable\n" \
"	}\n",

TEST_DIR"/flow/unreachable_in_func.sk",

false,

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
"}\n"
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

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	external func() Int\n" \
"	\n" \
"	mut x := func()\n" \
"	\n" \
"	return x\n",

TEST_DIR"/function/assign_return_value.sk",

false,

"; ModuleID = './test/sh/function/assign_return_value.sk'\n" \
"source_filename = \"./test/sh/function/assign_return_value.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.assign_return_value() {\n" \
"entry:\n" \
"  %0 = call i64 @func()\n" \
"  store i64 %0, i64* @x\n" \
"  %1 = load i64, i64* @x\n" \
"  ret i64 %1\n" \
"}\n" \
"\n" \
"declare i64 @func()\n"
);


pass |= e2e_wrapper(
"	external puts(s: Str)\n" \
"	\n" \
"	puts(\"hello world!\")\n",

TEST_DIR"/function/call_str.sk",

false,

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
"declare void @puts(i8*)\n"
);


pass |= e2e_wrapper(
"	f() {\n" \
		"# do nothing\n" \
"	}\n",

TEST_DIR"/function/declare_func.sk",

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	x() { noop }\n",

TEST_DIR"/function/empty_func_no_err.sk",

false,

"; ModuleID = './test/sh/function/empty_func_no_err.sk'\n" \
"source_filename = \"./test/sh/function/empty_func_no_err.sk\"\n" \
"\n" \
"define i64 @.empty_func_no_err() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private void @x() {\n" \
"entry:\n" \
"  ret void\n" \
"}\n"
);


pass |= e2e_wrapper(
"	export f() {\n" \
		"# do nothing\n" \
"	}\n",

TEST_DIR"/function/exported.sk",

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	f(num: Int) {\n" \
		"# do nothing\n" \
"	}\n" \
"	\n" \
"	f(1 + 2)\n",

TEST_DIR"/function/expr_param.sk",

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	external x() Int\n" \
"	external y(num: Int) Int\n",
TEST_DIR"/function/external_return_type.sk",

false,

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
"declare i64 @y(i64)\n"
);


pass |= e2e_wrapper(
"	external x(a: Int)\n",

TEST_DIR"/function/external_with_param.sk",

false,

"; ModuleID = './test/sh/function/external_with_param.sk'\n" \
"source_filename = \"./test/sh/function/external_with_param.sk\"\n" \
"\n" \
"define i64 @.external_with_param() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare void @x(i64)\n"
);


pass |= e2e_wrapper(
"	f(x: Int) Int {\n" \
		"return x\n" \
"	}\n" \
"	\n" \
"	f(f(1))\n",

TEST_DIR"/function/func_expr_param.sk",

false,

"; ModuleID = './test/sh/function/func_expr_param.sk'\n" \
"source_filename = \"./test/sh/function/func_expr_param.sk\"\n" \
"\n" \
"define i64 @.func_expr_param() {\n" \
"entry:\n" \
"  %0 = call i64 @f(i64 1)\n" \
"  %1 = call i64 @f(i64 %0)\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private i64 @f(i64 %0) {\n" \
"entry:\n" \
"  ret i64 %0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	f() {\n" \
		"mut x := 1\n" \
"	}\n" \
"	\n" \
"	mut x := 0\n",

TEST_DIR"/function/func_with_scope.sk",

false,

"; ModuleID = './test/sh/function/func_with_scope.sk'\n" \
"source_filename = \"./test/sh/function/func_with_scope.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.func_with_scope() {\n" \
"entry:\n" \
"  store i64 0, i64* @x\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private void @f() {\n" \
"entry:\n" \
"  %x = alloca i64\n" \
"  store i64 1, i64* %x\n" \
"  ret void\n" \
"}\n"
);


pass |= e2e_wrapper(
"	f(a: Int, b: Float) {\n" \
		"# do nothing\n" \
"	}\n",

TEST_DIR"/function/multi_param_func_different_types.sk",

false,

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
"}\n"
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

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	external a()\n" \
"	external b()\n" \
"	\n" \
"	a()\n",

TEST_DIR"/function/multiple_externals.sk",

false,

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
"declare void @b()\n"
);


pass |= e2e_wrapper(
"	f(num: Int) Int {\n" \
		"return num\n" \
"	}\n" \
"	\n" \
"	f(1)\n",

TEST_DIR"/function/multi_var_func.sk",

false,

"; ModuleID = './test/sh/function/multi_var_func.sk'\n" \
"source_filename = \"./test/sh/function/multi_var_func.sk\"\n" \
"\n" \
"define i64 @.multi_var_func() {\n" \
"entry:\n" \
"  %0 = call i64 @f(i64 1)\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private i64 @f(i64 %0) {\n" \
"entry:\n" \
"  ret i64 %0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	f() {\n" \
		"if true {\n" \
			"# do nothing\n" \
		"}\n" \
"	}\n",

TEST_DIR"/function/nested_if.sk",

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	external x()\n" \
"	\n" \
"	x()\n",

TEST_DIR"/function/no_args.sk",

false,

"; ModuleID = './test/sh/function/no_args.sk'\n" \
"source_filename = \"./test/sh/function/no_args.sk\"\n" \
"\n" \
"define i64 @.no_args() {\n" \
"entry:\n" \
"  call void @x()\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare void @x()\n"
);


pass |= e2e_wrapper(
"	external x()\n" \
"	\n" \
"	x()\n" \
"	\n" \
"	return 1\n",

TEST_DIR"/function/nodes_after_call_not_ignored.sk",

false,

"; ModuleID = './test/sh/function/nodes_after_call_not_ignored.sk'\n" \
"source_filename = \"./test/sh/function/nodes_after_call_not_ignored.sk\"\n" \
"\n" \
"define i64 @.nodes_after_call_not_ignored() {\n" \
"entry:\n" \
"  call void @x()\n" \
"  ret i64 1\n" \
"}\n" \
"\n" \
"declare void @x()\n"
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

false,

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
"  %ret = alloca double\n" \
"  store double 3.140000e+00, double* %ret\n" \
"  %0 = load double, double* %ret\n" \
"  ret double %0\n" \
"}\n" \
"\n" \
"define private double @func2() {\n" \
"entry:\n" \
"  ret double 3.140000e+00\n" \
"}\n"
);


pass |= e2e_wrapper(
"	external x(a: Int)\n" \
"	\n" \
"	x(1)\n",

TEST_DIR"/function/single_param.sk",

false,

"; ModuleID = './test/sh/function/single_param.sk'\n" \
"source_filename = \"./test/sh/function/single_param.sk\"\n" \
"\n" \
"define i64 @.single_param() {\n" \
"entry:\n" \
"  call void @x(i64 1)\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare void @x(i64)\n"
);


pass |= e2e_wrapper(
"	external f(x: Int)\n" \
"	\n" \
"	mut num := 1234\n" \
"	\n" \
"	f(num)\n",

TEST_DIR"/function/var_param.sk",

false,

"; ModuleID = './test/sh/function/var_param.sk'\n" \
"source_filename = \"./test/sh/function/var_param.sk\"\n" \
"\n" \
"@num = private global i64 0\n" \
"\n" \
"define i64 @.var_param() {\n" \
"entry:\n" \
"  store i64 1234, i64* @num\n" \
"  %0 = load i64, i64* @num\n" \
"  call void @f(i64 %0)\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare void @f(i64)\n"
);


pass |= e2e_wrapper(
"	f() {\n" \
		"return\n" \
"	}\n",

TEST_DIR"/function/void_explicit_ret.sk",

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	# comment\n" \
"	x := 0\n",

TEST_DIR"/misc/comment_then_code.sk",

false,

"; ModuleID = './test/sh/misc/comment_then_code.sk'\n" \
"source_filename = \"./test/sh/misc/comment_then_code.sk\"\n" \
"\n" \
"define i64 @.comment_then_code() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n"
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

false,

"; ModuleID = './test/sh/misc/const_inline.sk'\n" \
"source_filename = \"./test/sh/misc/const_inline.sk\"\n" \
"\n" \
"define i64 @.const_inline() {\n" \
"entry:\n" \
"  ret i64 3\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := \"\\n\"\n" \
"	mut y := '\\n'\n",

TEST_DIR"/misc/escape_sequences.sk",

false,

"; ModuleID = './test/sh/misc/escape_sequences.sk'\n" \
"source_filename = \"./test/sh/misc/escape_sequences.sk\"\n" \
"\n" \
"@0 = private unnamed_addr constant [2 x i8] c\"\\0A\\00\", align 1\n" \
"@x = private global i8* null\n" \
"@y = private global i32 0\n" \
"\n" \
"define i64 @.escape_sequences() {\n" \
"entry:\n" \
"  store i8* getelementptr inbounds ([2 x i8], [2 x i8]* @0, i32 0, i32 0), i8** @x\n" \
"  store i32 10, i32* @y\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut is_this_kept := true\n" \
"	#{\n" \
"	mut and_this := false\n" \
"	#}\n" \
"	mut what_about_this := true\n",

TEST_DIR"/misc/ignore_block_comments.sk",

false,

"; ModuleID = './test/sh/misc/ignore_block_comments.sk'\n" \
"source_filename = \"./test/sh/misc/ignore_block_comments.sk\"\n" \
"\n" \
"@is_this_kept = private global i1 false\n" \
"@what_about_this = private global i1 false\n" \
"\n" \
"define i64 @.ignore_block_comments() {\n" \
"entry:\n" \
"  store i1 true, i1* @is_this_kept\n" \
"  store i1 true, i1* @what_about_this\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	# comment will not cause any issues\n" \
"	\n" \
"	return 0\n",

TEST_DIR"/misc/ignore_comments.sk",

false,

"; ModuleID = './test/sh/misc/ignore_comments.sk'\n" \
"source_filename = \"./test/sh/misc/ignore_comments.sk\"\n" \
"\n" \
"define i64 @.ignore_comments() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	emoji := \"😀\"\n",

TEST_DIR"/misc/locale.sk",

false,

"; ModuleID = './test/sh/misc/locale.sk'\n" \
"source_filename = \"./test/sh/misc/locale.sk\"\n" \
"\n" \
"@0 = private unnamed_addr constant [5 x i8] c\"\\F0\\9F\\98\\80\\00\", align 1\n" \
"\n" \
"define i64 @.locale() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	#{\n" \
"	#{\n" \
"	#}\n",

TEST_DIR"/misc/nested_block_comment.sk",

false,

"; ModuleID = './test/sh/misc/nested_block_comment.sk'\n" \
"source_filename = \"./test/sh/misc/nested_block_comment.sk\"\n" \
"\n" \
"define i64 @.nested_block_comment() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	return 0",

TEST_DIR"/misc/no_eol_works.sk",

false,

"; ModuleID = './test/sh/misc/no_eol_works.sk'\n" \
"source_filename = \"./test/sh/misc/no_eol_works.sk\"\n" \
"\n" \
"define i64 @.no_eol_works() {\n" \
"entry:\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	if true {\n" \
		"noop\n" \
"	}\n" \
"	\n" \
"	noop\n",

TEST_DIR"/misc/noop.sk",

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	if false {\n" \
		"x := Int\n" \
"	}\n",

TEST_DIR"/misc/not_top_lvl_alias.sk",

false,

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
"}\n"
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

false,

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
"}\n"
);


pass |= e2e_wrapper(
"	f(a: Int, b: Int, c: Int) { noop }\n",

TEST_DIR"/misc/triple_param_no_overflow.sk",

false,

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
"}\n"
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

false,

"; ModuleID = './test/sh/misc/type_alias.sk'\n" \
"source_filename = \"./test/sh/misc/type_alias.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.type_alias() {\n" \
"entry:\n" \
"  store i64 1234, i64* @x\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"define private i64 @f(i64 %0) {\n" \
"entry:\n" \
"  ret i64 %0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 2.0 + 3.0\n",

TEST_DIR"/oper/add_float_consts.sk",

false,

"; ModuleID = './test/sh/oper/add_float_consts.sk'\n" \
"source_filename = \"./test/sh/oper/add_float_consts.sk\"\n" \
"\n" \
"@x = private global double 0.000000e+00\n" \
"\n" \
"define i64 @.add_float_consts() {\n" \
"entry:\n" \
"  store double 5.000000e+00, double* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 1 + 2\n",

TEST_DIR"/oper/add_int_consts.sk",

false,

"; ModuleID = './test/sh/oper/add_int_consts.sk'\n" \
"source_filename = \"./test/sh/oper/add_int_consts.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.add_int_consts() {\n" \
"entry:\n" \
"  store i64 3, i64* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 4.0 / 2.0\n",

TEST_DIR"/oper/div_float_consts.sk",

false,

"; ModuleID = './test/sh/oper/div_float_consts.sk'\n" \
"source_filename = \"./test/sh/oper/div_float_consts.sk\"\n" \
"\n" \
"@x = private global double 0.000000e+00\n" \
"\n" \
"define i64 @.div_float_consts() {\n" \
"entry:\n" \
"  store double 2.000000e+00, double* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 4 / 2\n",

TEST_DIR"/oper/div_int_consts.sk",

false,

"; ModuleID = './test/sh/oper/div_int_consts.sk'\n" \
"source_filename = \"./test/sh/oper/div_int_consts.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.div_int_consts() {\n" \
"entry:\n" \
"  store i64 2, i64* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	external f() Int\n" \
"	\n" \
"	x := f() / f()\n",

TEST_DIR"/oper/div_with_expr.sk",

false,

"; ModuleID = './test/sh/oper/div_with_expr.sk'\n" \
"source_filename = \"./test/sh/oper/div_with_expr.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.div_with_expr() {\n" \
"entry:\n" \
"  %0 = call i64 @f()\n" \
"  %1 = call i64 @f()\n" \
"  %2 = sdiv exact i64 %0, %1\n" \
"  store i64 %2, i64* @x\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare i64 @f()\n"
);


pass |= e2e_wrapper(
"	mut x := 2.0 ^ 3.0\n",

TEST_DIR"/oper/float_pow.sk",

false,

"; ModuleID = './test/sh/oper/float_pow.sk'\n" \
"source_filename = \"./test/sh/oper/float_pow.sk\"\n" \
"\n" \
"@x = private global double 0.000000e+00\n" \
"\n" \
"define i64 @.float_pow() {\n" \
"entry:\n" \
"  %0 = call double @_float_pow(double 2.000000e+00, double 3.000000e+00)\n" \
"  store double %0, double* @x\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare double @_float_pow(double, double)\n"
);


pass |= e2e_wrapper(
"	external f() Int\n" \
"	\n" \
"	mut x := f() + 1\n",

TEST_DIR"/oper/func_lhs.sk",

false,

"; ModuleID = './test/sh/oper/func_lhs.sk'\n" \
"source_filename = \"./test/sh/oper/func_lhs.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.func_lhs() {\n" \
"entry:\n" \
"  %0 = call i64 @f()\n" \
"  %1 = add nsw i64 %0, 1\n" \
"  store i64 %1, i64* @x\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare i64 @f()\n"
);


pass |= e2e_wrapper(
"	external f() Int\n" \
"	\n" \
"	mut x := 1 + f()\n",

TEST_DIR"/oper/func_rhs.sk",

false,

"; ModuleID = './test/sh/oper/func_rhs.sk'\n" \
"source_filename = \"./test/sh/oper/func_rhs.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.func_rhs() {\n" \
"entry:\n" \
"  %0 = call i64 @f()\n" \
"  %1 = add nsw i64 1, %0\n" \
"  store i64 %1, i64* @x\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare i64 @f()\n"
);


pass |= e2e_wrapper(
"	mut x := 10 ^ 3\n",

TEST_DIR"/oper/int_pow.sk",

false,

"; ModuleID = './test/sh/oper/int_pow.sk'\n" \
"source_filename = \"./test/sh/oper/int_pow.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.int_pow() {\n" \
"entry:\n" \
"  %0 = call i64 @_int_pow(i64 10, i64 3)\n" \
"  store i64 %0, i64* @x\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare i64 @_int_pow(i64, i64)\n"
);


pass |= e2e_wrapper(
"	mut x := 3.14 is 3.14\n",

TEST_DIR"/oper/is_cmp_float.sk",

false,

"; ModuleID = './test/sh/oper/is_cmp_float.sk'\n" \
"source_filename = \"./test/sh/oper/is_cmp_float.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.is_cmp_float() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 1234 is 1234\n",

TEST_DIR"/oper/is_cmp_int.sk",

false,

"; ModuleID = './test/sh/oper/is_cmp_int.sk'\n" \
"source_filename = \"./test/sh/oper/is_cmp_int.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.is_cmp_int() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 'x' is 'x'\n",

TEST_DIR"/oper/is_cmp_rune.sk",

false,

"; ModuleID = './test/sh/oper/is_cmp_rune.sk'\n" \
"source_filename = \"./test/sh/oper/is_cmp_rune.sk\"\n" \
"\n" \
"@x = private global i1 false\n" \
"\n" \
"define i64 @.is_cmp_rune() {\n" \
"entry:\n" \
"  store i1 true, i1* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := \"x\" is \"x\"\n",

TEST_DIR"/oper/is_cmp_str.sk",

false,

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
"  store i1 %0, i1* @x\n" \
"  ret i64 0\n" \
"}\n" \
"\n" \
"declare i1 @_strcmp(i8*, i8*)\n"
);


pass |= e2e_wrapper(
"	mut x := 1\n" \
"	mut y := x + 2\n" \
"	mut z := x < 1\n",

TEST_DIR"/oper/lhs_with_var.sk",

false,

"; ModuleID = './test/sh/oper/lhs_with_var.sk'\n" \
"source_filename = \"./test/sh/oper/lhs_with_var.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"@y = private global i64 0\n" \
"@z = private global i1 false\n" \
"\n" \
"define i64 @.lhs_with_var() {\n" \
"entry:\n" \
"  store i64 1, i64* @x\n" \
"  %0 = load i64, i64* @x\n" \
"  %1 = add nsw i64 %0, 2\n" \
"  store i64 %1, i64* @y\n" \
"  %2 = load i64, i64* @x\n" \
"  %3 = icmp slt i64 %2, 1\n" \
"  store i1 %3, i1* @z\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 1 << 2\n",

TEST_DIR"/oper/lshift.sk",

false,

"; ModuleID = './test/sh/oper/lshift.sk'\n" \
"source_filename = \"./test/sh/oper/lshift.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.lshift() {\n" \
"entry:\n" \
"  store i64 4, i64* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 9 mod 2\n" \
"	mut y := -10 mod 3\n" \
"	mut z := 12.0 mod 5.0\n",

TEST_DIR"/oper/mod.sk",

false,

"; ModuleID = './test/sh/oper/mod.sk'\n" \
"source_filename = \"./test/sh/oper/mod.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"@y = private global i64 0\n" \
"@z = private global double 0.000000e+00\n" \
"\n" \
"define i64 @.mod() {\n" \
"entry:\n" \
"  store i64 1, i64* @x\n" \
"  store i64 -1, i64* @y\n" \
"  store double 2.000000e+00, double* @z\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 2.0 * 3.0\n",

TEST_DIR"/oper/mult_float_consts.sk",

false,

"; ModuleID = './test/sh/oper/mult_float_consts.sk'\n" \
"source_filename = \"./test/sh/oper/mult_float_consts.sk\"\n" \
"\n" \
"@x = private global double 0.000000e+00\n" \
"\n" \
"define i64 @.mult_float_consts() {\n" \
"entry:\n" \
"  store double 6.000000e+00, double* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 2 * 3\n",

TEST_DIR"/oper/mult_int_consts.sk",

false,

"; ModuleID = './test/sh/oper/mult_int_consts.sk'\n" \
"source_filename = \"./test/sh/oper/mult_int_consts.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.mult_int_consts() {\n" \
"entry:\n" \
"  store i64 6, i64* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	x := 1\n" \
"	\n" \
"	return - x\n",

TEST_DIR"/oper/negative_unary.sk",

false,

"; ModuleID = './test/sh/oper/negative_unary.sk'\n" \
"source_filename = \"./test/sh/oper/negative_unary.sk\"\n" \
"\n" \
"define i64 @.negative_unary() {\n" \
"entry:\n" \
"  ret i64 -1\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := (1 + 2)\n" \
"	\n" \
"	mut y := (((1 + 2)))\n",

TEST_DIR"/oper/paren_expr.sk",

false,

"; ModuleID = './test/sh/oper/paren_expr.sk'\n" \
"source_filename = \"./test/sh/oper/paren_expr.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"@y = private global i64 0\n" \
"\n" \
"define i64 @.paren_expr() {\n" \
"entry:\n" \
"  store i64 3, i64* @x\n" \
"  store i64 3, i64* @y\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 1 + (2 + 3)\n",

TEST_DIR"/oper/paren_rhs_expr.sk",

false,

"; ModuleID = './test/sh/oper/paren_rhs_expr.sk'\n" \
"source_filename = \"./test/sh/oper/paren_rhs_expr.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.paren_rhs_expr() {\n" \
"entry:\n" \
"  store i64 6, i64* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 1\n" \
"	\n" \
"	mut z := 2 + x\n",

TEST_DIR"/oper/rhs_with_var.sk",

false,

"; ModuleID = './test/sh/oper/rhs_with_var.sk'\n" \
"source_filename = \"./test/sh/oper/rhs_with_var.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"@z = private global i64 0\n" \
"\n" \
"define i64 @.rhs_with_var() {\n" \
"entry:\n" \
"  store i64 1, i64* @x\n" \
"  %0 = load i64, i64* @x\n" \
"  %1 = add nsw i64 2, %0\n" \
"  store i64 %1, i64* @z\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	return 0b1111 >> 2\n",

TEST_DIR"/oper/rshift.sk",

false,

"; ModuleID = './test/sh/oper/rshift.sk'\n" \
"source_filename = \"./test/sh/oper/rshift.sk\"\n" \
"\n" \
"define i64 @.rshift() {\n" \
"entry:\n" \
"  ret i64 3\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 2.0 - 3.0\n",

TEST_DIR"/oper/sub_float_consts.sk",

false,

"; ModuleID = './test/sh/oper/sub_float_consts.sk'\n" \
"source_filename = \"./test/sh/oper/sub_float_consts.sk\"\n" \
"\n" \
"@x = private global double 0.000000e+00\n" \
"\n" \
"define i64 @.sub_float_consts() {\n" \
"entry:\n" \
"  store double -1.000000e+00, double* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut x := 1 - 2\n",

TEST_DIR"/oper/sub_int_consts.sk",

false,

"; ModuleID = './test/sh/oper/sub_int_consts.sk'\n" \
"source_filename = \"./test/sh/oper/sub_int_consts.sk\"\n" \
"\n" \
"@x = private global i64 0\n" \
"\n" \
"define i64 @.sub_int_consts() {\n" \
"entry:\n" \
"  store i64 -1, i64* @x\n" \
"  ret i64 0\n" \
"}\n"
);


pass |= e2e_wrapper(
"	mut w := - 1.0\n" \
"	mut x := not not false\n" \
"	mut y := - 1\n" \
"	mut z := - y\n",

TEST_DIR"/oper/unary_expr.sk",

false,

"; ModuleID = './test/sh/oper/unary_expr.sk'\n" \
"source_filename = \"./test/sh/oper/unary_expr.sk\"\n" \
"\n" \
"@w = private global double 0.000000e+00\n" \
"@x = private global i1 false\n" \
"@y = private global i64 0\n" \
"@z = private global i64 0\n" \
"\n" \
"define i64 @.unary_expr() {\n" \
"entry:\n" \
"  store double -1.000000e+00, double* @w\n" \
"  store i1 false, i1* @x\n" \
"  store i64 -1, i64* @y\n" \
"  %0 = load i64, i64* @y\n" \
"  %1 = sub nsw i64 0, %0\n" \
"  store i64 %1, i64* @z\n" \
"  ret i64 0\n" \
"}\n"
);


// error tests

pass |= e2e_wrapper(
"x := 0\n" \
"\n" \
"z: Bool = x\n",
TEST_DIR"/error/declare/assign_mismatch_var_type.sk",

true,

"./test/sh/error/declare/assign_mismatch_var_type.sk: Compilation error: line 3 column 11: expected type \"Bool\", got \"Int\"\n" \
"./test/sh/error/declare/assign_mismatch_var_type.sk: Warning: line 1 column 1: variable \"x\" is unused\n" \
"./test/sh/error/declare/assign_mismatch_var_type.sk: Warning: line 3 column 1: variable \"z\" is unused\n"
);

pass |= e2e_wrapper(
"x: Int =\n",
TEST_DIR"/error/declare/assign_missing_rhs.sk",

true,

"./test/sh/error/declare/assign_missing_rhs.sk: Compilation error: line 1 column 9: expected expression in assignment variable\n"
);



pass |= e2e_wrapper(
"mut x := \"this will\" / \"fail\"\n",
TEST_DIR"/error/declare/assign_unassignable_expression.sk",

true,

"./test/sh/error/declare/assign_unassignable_expression.sk: Compilation error: line 1 column 10: expected a numeric value\n" \
"./test/sh/error/declare/assign_unassignable_expression.sk: Warning: line 1 column 5: variable \"x\" should be constant\n" \
"./test/sh/error/declare/assign_unassignable_expression.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := y\n",
TEST_DIR"/error/declare/assign_unknown_var.sk",

true,

"./test/sh/error/declare/assign_unknown_var.sk: Compilation error: line 1 column 6: variable \"y\" not found\n"
);


pass |= e2e_wrapper(
"x :=\n",
TEST_DIR"/error/declare/auto_assign_missing_rhs.sk",

true,

"./test/sh/error/declare/auto_assign_missing_rhs.sk: Compilation error: line 1 column 5: expected expression in assignment variable\n"
);


pass |= e2e_wrapper(
"x: fail = 1\n",
TEST_DIR"/error/declare/bad_type_not_allowed.sk",

true,

"./test/sh/error/declare/bad_type_not_allowed.sk: Compilation error: line 1 column 4: type \"fail\" could not be found\n"
);


pass |= e2e_wrapper(
"mut x: Int = 1 is 1\n",
TEST_DIR"/error/declare/check_bool_assign.sk",

true,

"./test/sh/error/declare/check_bool_assign.sk: Compilation error: line 1 column 14: expected type \"Int\", got \"Bool\"\n" \
"./test/sh/error/declare/check_bool_assign.sk: Warning: line 1 column 5: variable \"x\" should be constant\n" \
"./test/sh/error/declare/check_bool_assign.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"mut x: Int = not true\n",
TEST_DIR"/error/declare/check_bool_not_assign.sk",

true,

"./test/sh/error/declare/check_bool_not_assign.sk: Compilation error: line 1 column 18: expected type \"Int\", got \"Bool\"\n" \
"./test/sh/error/declare/check_bool_not_assign.sk: Warning: line 1 column 5: variable \"x\" should be constant\n" \
"./test/sh/error/declare/check_bool_not_assign.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := '\n" \
"'\n",
TEST_DIR"/error/declare/control_char_in_rune.sk",

true,

"./test/sh/error/declare/control_char_in_rune.sk: Compilation error: line 1 column 6: control character cannot be used in rune\n" \
"./test/sh/error/declare/control_char_in_rune.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x: Float = 1 + 2\n",
TEST_DIR"/error/declare/expr_assign_bad_type.sk",

true,

"./test/sh/error/declare/expr_assign_bad_type.sk: Compilation error: line 1 column 12: expected type \"Float\", got \"Int\"\n" \
"./test/sh/error/declare/expr_assign_bad_type.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0\n",
TEST_DIR"/error/declare/float_overflow.sk",

true,

"./test/sh/error/declare/float_overflow.sk: Compilation error: line 1 column 6: overflow occurred while parsing \"999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0\"\n" \
"./test/sh/error/declare/float_overflow.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 99999999999999999999999999999999\n",
TEST_DIR"/error/declare/int_overflow.sk",

true,

"./test/sh/error/declare/int_overflow.sk: Compilation error: line 1 column 6: overflow occurred while parsing \"99999999999999999999999999999999\"\n" \
"./test/sh/error/declare/int_overflow.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := -99999999999999999999999999999999\n",
TEST_DIR"/error/declare/int_underflow.sk",

true,

"./test/sh/error/declare/int_underflow.sk: Compilation error: line 1 column 6: overflow occurred while parsing \"-99999999999999999999999999999999\"\n" \
"./test/sh/error/declare/int_underflow.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 0\n" \
"x = 1\n",
TEST_DIR"/error/declare/reassign_const.sk",

true,

"./test/sh/error/declare/reassign_const.sk: Compilation error: line 2 column 1: cannot reassign const variable \"x\"\n" \
"./test/sh/error/declare/reassign_const.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"mut x := 0\n" \
"x =\n",
TEST_DIR"/error/declare/reassign_missing_rhs.sk",

true,

"./test/sh/error/declare/reassign_missing_rhs.sk: Compilation error: line 2 column 4: expected expression in assignment variable\n"
);


pass |= e2e_wrapper(
"x = 0\n",
TEST_DIR"/error/declare/reassign_non_existet_var.sk",

true,

"./test/sh/error/declare/reassign_non_existet_var.sk: Compilation error: line 1 column 1: variable \"x\" not found\n"
);


pass |= e2e_wrapper(
"external x()\n" \
"external x()\n",
TEST_DIR"/error/declare/redeclare_external.sk",

true,

"./test/sh/error/declare/redeclare_external.sk: Compilation error: line 2 column 10: cannot redeclare function \"x\"\n" \
"./test/sh/error/declare/redeclare_external.sk: Warning: line 1 column 10: function \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 0\n" \
"x := 0\n",
TEST_DIR"/error/declare/var_already_defined.sk",

true,

"./test/sh/error/declare/var_already_defined.sk: Compilation error: line 2 column 1: variable \"x\" already defined\n" \
"./test/sh/error/declare/var_already_defined.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 1\n" \
"x := Int\n",
TEST_DIR"/error/declare/var_as_alias.sk",

true,

"./test/sh/error/declare/var_as_alias.sk: Compilation error: line 2 column 1: cannot redeclare variable \"x\" as type alias\n" \
"./test/sh/error/declare/var_as_alias.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"# this wont work\n" \
"else {\n" \
"	noop\n" \
"}\n",
TEST_DIR"/error/flow/comment_then_else.sk",

true,

"./test/sh/error/flow/comment_then_else.sk: Compilation error: line 1 column 1: else/elif statement missing preceding if statement\n"
);


pass |= e2e_wrapper(
"else {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/flow/else_without_if.sk",

true,

"./test/sh/error/flow/else_without_if.sk: Compilation error: line 1 column 1: else/elif statement missing preceding if statement\n"
);


pass |= e2e_wrapper(
"if true {}\n",
TEST_DIR"/error/flow/empty_block.sk",

true,

"./test/sh/error/flow/empty_block.sk: Compilation error: line 1 column 10: code block cannot be empty\n"
);


pass |= e2e_wrapper(
"if true\n",
TEST_DIR"/error/flow/if_missing_block.sk",

true,

"./test/sh/error/flow/if_missing_block.sk: Compilation error: line 1 column 1: if statement must be followed by code block\n"
);


pass |= e2e_wrapper(
"if true {\n" \
"	mut x := 0\n" \
"}\n" \
"\n" \
"return x\n",
TEST_DIR"/error/flow/if_var_out_of_scope.sk",

true,

"./test/sh/error/flow/if_var_out_of_scope.sk: Compilation error: line 5 column 8: variable \"x\" not found\n" \
"./test/sh/error/flow/if_var_out_of_scope.sk: Warning: line 2 column 6: variable \"x\" should be constant\n" \
"./test/sh/error/flow/if_var_out_of_scope.sk: Warning: line 2 column 6: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"if x {\n" \
"	return 1\n" \
"}\n" \
"\n" \
"return 0\n",
TEST_DIR"/error/flow/if_with_nonexisting_var.sk",

true,

"./test/sh/error/flow/if_with_nonexisting_var.sk: Compilation error: line 1 column 4: variable \"x\" not found\n"
);


pass |= e2e_wrapper(
"if true {\n" \
"	x 1\n" \
"}\n" \
"\n" \
"return 2\n",
TEST_DIR"/error/flow/invalid_return_in_if.sk",

true,

"./test/sh/error/flow/invalid_return_in_if.sk: Compilation error: line 2 column 2: expression cannot be used on its own\n"
);


pass |= e2e_wrapper(
"if # this will fail\n",
TEST_DIR"/error/flow/missing_expr.sk",

true,

"./test/sh/error/flow/missing_expr.sk: Compilation error: line 1 column 4: invalid expression near \"# this will fail\"\n"
);


pass |= e2e_wrapper(
"return 1.0\n",
TEST_DIR"/error/flow/non_int_return.sk",

true,

"./test/sh/error/flow/non_int_return.sk: Compilation error: line 1 column 8: returning non-int expression \"1.0\" from main\n"
);


pass |= e2e_wrapper(
"return 0xffffffffffffffff\n",
TEST_DIR"/error/flow/return_int_overflow.sk",

true,

"./test/sh/error/flow/return_int_overflow.sk: Compilation error: line 1 column 8: overflow occurred while parsing \"0xffffffffffffffff\"\n"
);


pass |= e2e_wrapper(
"return # wont work\n",
TEST_DIR"/error/flow/return_missing_expr.sk",

true,

"./test/sh/error/flow/return_missing_expr.sk: Compilation error: line 1 column 1: expected expression in return\n"
);


pass |= e2e_wrapper(
"return x\n",
TEST_DIR"/error/flow/return_non_existent_var.sk",

true,

"./test/sh/error/flow/return_non_existent_var.sk: Compilation error: line 1 column 8: variable \"x\" not found\n"
);


pass |= e2e_wrapper(
"if true {\n" \
"	noop\n" \
"}\n" \
"x := 1\n" \
"elif true {\n" \
"	noop\n" \
"}\n",
TEST_DIR"/error/flow/stmt_between_elif.sk",

true,

"./test/sh/error/flow/stmt_between_elif.sk: Compilation error: line 5 column 1: else/elif statement missing preceding if statement\n" \
"./test/sh/error/flow/stmt_between_elif.sk: Warning: line 4 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"f() {\n" \
"	return\n" \
"	x := 1\n" \
"}\n",
TEST_DIR"/error/flow/unreachable_in_func.sk",

true,

"./test/sh/error/flow/unreachable_in_func.sk: Compilation error: line 3 column 2: unreachable code\n" \
"./test/sh/error/flow/unreachable_in_func.sk: Warning: line 1 column 1: function \"f\" is unused\n" \
"./test/sh/error/flow/unreachable_in_func.sk: Warning: line 3 column 2: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"return 1\n" \
"\n" \
"x := 0\n",
TEST_DIR"/error/flow/unreachable_return.sk",

true,

"./test/sh/error/flow/unreachable_return.sk: Compilation error: line 3 column 1: unreachable code\n" \
"./test/sh/error/flow/unreachable_return.sk: Warning: line 3 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := Int\n" \
"x() { noop }\n",
TEST_DIR"/error/func/alias_as_func.sk",

true,

"./test/sh/error/func/alias_as_func.sk: Compilation error: line 2 column 1: cannot redeclare type alias \"x\" as function\n"
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

true,

"./test/sh/error/func/assign_bad_type.sk: Compilation error: line 7 column 12: expected type \"Float\", got \"Int\"\n" \
"./test/sh/error/func/assign_bad_type.sk: Warning: line 7 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"f() {\n" \
"	# do nothing\n" \
"}\n" \
"\n" \
"x := f()\n",
TEST_DIR"/error/func/assign_void_ret.sk",

true,

"./test/sh/error/func/assign_void_ret.sk: Compilation error: line 5 column 6: function returning type void cannot be assigned to variable \"x\"\n"
);


pass |= e2e_wrapper(
"f(b: Bool) {\n" \
"	# do nothing\n" \
"}\n" \
"\n" \
"f(1 + 2)\n",
TEST_DIR"/error/func/bad_expr_param.sk",

true,

"./test/sh/error/func/bad_expr_param.sk: Compilation error: line 5 column 3: expected param of type \"Bool\", got \"Int\"\n" \
"./test/sh/error/func/bad_expr_param.sk: Warning: line 1 column 3: variable \"b\" is unused\n"
);


pass |= e2e_wrapper(
"external x(num: Int)\n" \
"\n" \
"x(false)\n",
TEST_DIR"/error/func/bad_param_type.sk",

true,

"./test/sh/error/func/bad_param_type.sk: Compilation error: line 3 column 3: expected param of type \"Int\", got \"Bool\"\n"
);


pass |= e2e_wrapper(
"x() { noop }\n" \
"x := Int\n",
TEST_DIR"/error/func/func_as_alias.sk",

true,

"./test/sh/error/func/func_as_alias.sk: Compilation error: line 2 column 1: cannot redeclare function \"x\" as type alias\n" \
"./test/sh/error/func/func_as_alias.sk: Warning: line 1 column 1: function \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x() { return }\n" \
"\n" \
"x := 1\n",
TEST_DIR"/error/func/func_as_var.sk",

true,

"./test/sh/error/func/func_as_var.sk: Compilation error: line 3 column 1: cannot redeclare function \"x\" as variable\n" \
"./test/sh/error/func/func_as_var.sk: Warning: line 1 column 1: function \"x\" is unused\n"
);


pass |= e2e_wrapper(
"f(x: Int) {\n" \
"	# do nothing\n" \
"}\n" \
"\n" \
"f(1, 2, 3)\n",
TEST_DIR"/error/func/invalid_num_params.sk",

true,

"./test/sh/error/func/invalid_num_params.sk: Compilation error: line 5 column 1: invalid number of parameters\n" \
"./test/sh/error/func/invalid_num_params.sk: Warning: line 1 column 3: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"return f(",
TEST_DIR"/error/func/missing_closing_paren.sk",

true,

"./test/sh/error/func/missing_closing_paren.sk: Compilation error: line 1 column 8: function call missing closing parenthesis\n"
);


pass |= e2e_wrapper(
"x := f()\n",
TEST_DIR"/error/func/missing_declaration.sk",

true,

"./test/sh/error/func/missing_declaration.sk: Compilation error: line 1 column 6: function \"f\" missing declaration\n"
);


pass |= e2e_wrapper(
"f() Int {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/func/missing_return.sk",

true,

"./test/sh/error/func/missing_return.sk: Compilation error: expected return value in function \"f\"\n" \
"./test/sh/error/func/missing_return.sk: Warning: line 1 column 1: function \"f\" is unused\n"
);


pass |= e2e_wrapper(
"external f(a: Int, b: Int)\n" \
"\n" \
"f(1 2)\n",
TEST_DIR"/error/func/multi_param_call_missing_comma.sk",

true,

"./test/sh/error/func/multi_param_call_missing_comma.sk: Compilation error: line 3 column 5: expected comma\n"
);


pass |= e2e_wrapper(
"f(x: Int y: Int) {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/func/multi_param_missing_comma.sk",

true,

"./test/sh/error/func/multi_param_missing_comma.sk: Compilation error: line 1 column 10: expected comma\n"
);


pass |= e2e_wrapper(
"external f(x: Int)\n" \
"\n" \
"x := false\n" \
"\n" \
"f(x)\n",
TEST_DIR"/error/func/param_bad_type.sk",

true,

"./test/sh/error/func/param_bad_type.sk: Compilation error: line 3 column 1: variable \"x\" already defined\n" \
"./test/sh/error/func/param_bad_type.sk: Warning: line 1 column 10: function \"f\" is unused\n"
);


pass |= e2e_wrapper(
"external f(x: Int)\n" \
"\n" \
"f(fail)\n",
TEST_DIR"/error/func/param_unknown_var.sk",

true,

"./test/sh/error/func/param_unknown_var.sk: Compilation error: line 3 column 3: variable \"fail\" not found\n"
);


pass |= e2e_wrapper(
"f() {\n" \
"	# do nothing\n" \
"}\n" \
"\n" \
"f(x)\n",
TEST_DIR"/error/func/passing_to_paramless_func.sk",

true,

"./test/sh/error/func/passing_to_paramless_func.sk: Compilation error: line 5 column 1: invalid number of parameters\n"
);


pass |= e2e_wrapper(
"f(x: does_not_exist) {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/func/proto_param_bad_type.sk",

true,

"./test/sh/error/func/proto_param_bad_type.sk: Compilation error: type \"does_not_exist\" could not be found\n" \
"./test/sh/error/func/proto_param_bad_type.sk: Warning: line 1 column 1: function \"f\" is unused\n"
);


pass |= e2e_wrapper(
"f() does_not_exist {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/func/proto_return_bad_type.sk",

true,

"./test/sh/error/func/proto_return_bad_type.sk: Compilation error: type \"does_not_exist\" could not be found\n"
);


pass |= e2e_wrapper(
"f() Int {\n" \
"	return false\n" \
"}\n",
TEST_DIR"/error/func/return_type_mismatch.sk",

true,

"./test/sh/error/func/return_type_mismatch.sk: Compilation error: line 2 column 9: expected type \"Int\", got \"Bool\"\n" \
"./test/sh/error/func/return_type_mismatch.sk: Warning: line 1 column 1: function \"f\" is unused\n"
);


pass |= e2e_wrapper(
"x := 0\n" \
"\n" \
"f(x: Int) Int {\n" \
"	return x\n" \
"}\n",
TEST_DIR"/error/func/shadowed_param.sk",

true,

"./test/sh/error/func/shadowed_param.sk: Compilation error: line 3 column 3: variable \"x\" shadows existing variable\n" \
"./test/sh/error/func/shadowed_param.sk: Warning: line 1 column 1: variable \"x\" is unused\n" \
"./test/sh/error/func/shadowed_param.sk: Warning: line 3 column 1: function \"f\" is unused\n"
);


pass |= e2e_wrapper(
"f() {\n" \
"	return 0\n" \
"}\n" \
"\n" \
"f()\n",
TEST_DIR"/error/func/unexpected_return.sk",

true,

"./test/sh/error/func/unexpected_return.sk: Compilation error: unexpected return from void function \"f\"\n"
);


pass |= e2e_wrapper(
"x := 1\n" \
"\n" \
"x() { noop }\n",
TEST_DIR"/error/func/var_as_func.sk",

true,

"./test/sh/error/func/var_as_func.sk: Compilation error: line 3 column 1: cannot redeclare variable \"x\" as function\n" \
"./test/sh/error/func/var_as_func.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"external f()\n" \
"\n" \
"x := f(1)\n",
TEST_DIR"/error/func/var_def_check_func.sk",

true,

"./test/sh/error/func/var_def_check_func.sk: Compilation error: line 3 column 6: function returning type void cannot be assigned to variable \"x\"\n"
);


pass |= e2e_wrapper(
"x := Int\n" \
"x := 1\n",
TEST_DIR"/error/misc/alias_as_var.sk",

true,

"./test/sh/error/misc/alias_as_var.sk: Compilation error: line 2 column 1: cannot redeclare type alias \"x\" as variable\n"
);


pass |= e2e_wrapper(
"#{\n",
TEST_DIR"/error/misc/closing_block_comment_missing.sk",

true,

"./test/sh/error/misc/closing_block_comment_missing.sk: Compilation error: line 1 column 1: expected closing \"#}\" for block comment\n"
);


pass |= e2e_wrapper(
"if true {\n" \
"	return 1\n" \
"} {\n" \
"	return 2\n" \
"}\n" \
"\n" \
"return 0\n",
TEST_DIR"/error/misc/duplicated_code_block.sk",

true,

"./test/sh/error/misc/duplicated_code_block.sk: Compilation error: line 3 column 3: unexpected code block\n"
);


pass |= e2e_wrapper(
"if true { return 1 }}\n",
TEST_DIR"/error/misc/extra_close_bracket.sk",

true,

"./test/sh/error/misc/extra_close_bracket.sk: Compilation error: line 1 column 21: missing opening bracket\n"
);


pass |= e2e_wrapper(
"\xc3\x28\n",
TEST_DIR"/error/misc/illegal_utf8.sk",

true,

"./test/sh/error/misc/illegal_utf8.sk: Compilation error: illegal UTF8 sequence at character offset 0\n"
);


pass |= e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 {\n" \
"	# invalid\n" \
"}\n",
TEST_DIR"/error/misc/invalid_block_in_assign.sk",

true,

"./test/sh/error/misc/invalid_block_in_assign.sk: Compilation error: line 4 column 2: unexpected code block\n" \
"./test/sh/error/misc/invalid_block_in_assign.sk: Warning: line 1 column 5: variable \"x\" should be constant\n" \
"./test/sh/error/misc/invalid_block_in_assign.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 1\n" \
"{\n" \
"	# invalid\n" \
"}\n",
TEST_DIR"/error/misc/invalid_block_in_def.sk",

true,

"./test/sh/error/misc/invalid_block_in_def.sk: Compilation error: line 3 column 2: unexpected code block\n"
);


pass |= e2e_wrapper(
"return 1 {\n" \
"	# invalid\n" \
"}\n",
TEST_DIR"/error/misc/invalid_block_in_return.sk",

true,

"./test/sh/error/misc/invalid_block_in_return.sk: Compilation error: line 2 column 2: unexpected code block\n"
);


pass |= e2e_wrapper(
"#x\n",
TEST_DIR"/error/misc/invalid_comment_start.sk",

true,

"./test/sh/error/misc/invalid_comment_start.sk: Compilation error: line 1 column 1: invalid start of comment\n"
);


pass |= e2e_wrapper(
"x!@$ := \"this will fail\"\n",
TEST_DIR"/error/misc/invalid_identifier.sk",

true,

"./test/sh/error/misc/invalid_identifier.sk: Compilation error: line 1 column 1: unexpected token: \"x!@$\"\n"
);


pass |= e2e_wrapper(
"x := (_)\n",
TEST_DIR"/error/misc/invalid_paren_expr.sk",

true,

"./test/sh/error/misc/invalid_paren_expr.sk: Compilation error: line 1 column 7: invalid expression near \"_\"\n"
);


pass |= e2e_wrapper(
"x := '\\xdb80'\n",
TEST_DIR"/error/misc/invalid_utf_rune.sk",

true,

"./test/sh/error/misc/invalid_utf_rune.sk: Compilation error: line 1 column 6: illegal UTF8 sequence in this region\n" \
"./test/sh/error/misc/invalid_utf_rune.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := \"\\xdb80\"\n",
TEST_DIR"/error/misc/invalid_utf_str.sk",

true,

"./test/sh/error/misc/invalid_utf_str.sk: Compilation error: line 1 column 6: illegal UTF8 sequence in this region\n" \
"./test/sh/error/misc/invalid_utf_str.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"export main() {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/misc/main_reserved.sk",

true,

"./test/sh/error/misc/main_reserved.sk: Compilation error: line 1 column 8: cannot export function \"main\"\n"
);


pass |= e2e_wrapper(
"if true {{\n" \
"	return 1\n" \
"}\n" \
"\n" \
"return 0\n",
TEST_DIR"/error/misc/mismatched_bracket.sk",

true,

"./test/sh/error/misc/mismatched_bracket.sk: Compilation error: Reached EOF, expected closing bracket\n"
);


pass |= e2e_wrapper(
"x()\n",
TEST_DIR"/error/misc/missing_external.sk",

true,

"./test/sh/error/misc/missing_external.sk: Compilation error: line 1 column 1: function \"x\" missing declaration\n"
);


pass |= e2e_wrapper(
"x := \"\n",
TEST_DIR"/error/misc/missing_quote.sk",

true,

"./test/sh/error/misc/missing_quote.sk: Compilation error: line 1 column 6: expected closing quote\n"
);


pass |= e2e_wrapper(
"{\n",
TEST_DIR"/error/misc/no_closing_bracket.sk",

true,

"./test/sh/error/misc/no_closing_bracket.sk: Compilation error: Reached EOF, expected closing bracket\n"
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

true,

"./test/sh/error/misc/no_nested_func.sk: Compilation error: line 2 column 9: cannot declare nested function \"func\" as external or exported\n"
);


pass |= e2e_wrapper(
"external x() x()\n",
TEST_DIR"/error/misc/no_newline_external.sk",

true,

"./test/sh/error/misc/no_newline_external.sk: Compilation error: line 1 column 1: unexpected token: \"external\"\n"
);


pass |= e2e_wrapper(
"some_alias := Int\n" \
"some_alias := Float\n",
TEST_DIR"/error/misc/no_same_name_aliases.sk",

true,

"./test/sh/error/misc/no_same_name_aliases.sk: Compilation error: line 2 column 1: alias \"some_alias\" is already defined\n"
);


pass |= e2e_wrapper(
"{ noop }\n",
TEST_DIR"/error/misc/only_brackets.sk",

true,

"./test/sh/error/misc/only_brackets.sk: Compilation error: line 1 column 3: unexpected code block\n"
);


pass |= e2e_wrapper(
"x\n",
TEST_DIR"/error/misc/only_identifier_fails.sk",

true,

"./test/sh/error/misc/only_identifier_fails.sk: Compilation error: line 1 column 1: expression cannot be used on its own\n"
);


pass |= e2e_wrapper(
"x := (1\n",
TEST_DIR"/error/misc/paren_expr_missing_paren.sk",

true,

"./test/sh/error/misc/paren_expr_missing_paren.sk: Compilation error: line 1 column 8: missing closing parenthesis\n"
);


pass |= e2e_wrapper(
"}\n",
TEST_DIR"/error/misc/stray_close_bracket.sk",

true,

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

true,

"./test/sh/error/misc/token_iter_keep_loc.sk: Compilation error: line 12 column 1: expression cannot be used on its own\n" \
"./test/sh/error/misc/token_iter_keep_loc.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"# TODO: fix semantic layer not checking for trivial types\n" \
"\n" \
"x: Int = 1\n" \
"\n" \
"# explicitly cause file to fail so CI passes\n" \
"y := x + false\n",
TEST_DIR"/error/misc/trivial_type.sk",

true,

"./test/sh/error/misc/trivial_type.sk: Compilation error: line 6 column 10: expected type \"Int\", got \"Bool\"\n" \
"./test/sh/error/misc/trivial_type.sk: Warning: line 3 column 1: variable \"x\" is unused\n" \
"./test/sh/error/misc/trivial_type.sk: Warning: line 6 column 1: variable \"y\" is unused\n"
);


pass |= e2e_wrapper(
"# do nothing\n" \
"\n" \
"{\n" \
"	# not allowed\n" \
"}\n",
TEST_DIR"/error/misc/unexpected_code_block.sk",

true,

"./test/sh/error/misc/unexpected_code_block.sk: Compilation error: line 4 column 2: unexpected code block\n"
);


pass |= e2e_wrapper(
"\"this will fail\"\n",
TEST_DIR"/error/misc/unexpected_str_fails.sk",

true,

"./test/sh/error/misc/unexpected_str_fails.sk: Compilation error: line 1 column 1: expression cannot be used on its own\n"
);


pass |= e2e_wrapper(
"this_will_fail\n",
TEST_DIR"/error/misc/unexpected_token_fails.sk",

true,

"./test/sh/error/misc/unexpected_token_fails.sk: Compilation error: line 1 column 1: expression cannot be used on its own\n"
);


pass |= e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 + \"fail\"\n",
TEST_DIR"/error/oper/add_mismatched_consts.sk",

true,

"./test/sh/error/oper/add_mismatched_consts.sk: Compilation error: line 3 column 9: expected type \"Int\", got \"Str\"\n" \
"./test/sh/error/oper/add_mismatched_consts.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x: Float = 1\n",
TEST_DIR"/error/oper/assign_bad_type.sk",

true,

"./test/sh/error/oper/assign_bad_type.sk: Compilation error: line 1 column 12: expected type \"Float\", got \"Int\"\n" \
"./test/sh/error/oper/assign_bad_type.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"mut x := 1 and 1\n",
TEST_DIR"/error/oper/bool_expr_and_not_bool.sk",

true,

"./test/sh/error/oper/bool_expr_and_not_bool.sk: Compilation error: line 1 column 10: expected type \"Bool\", got \"Int\"\n" \
"./test/sh/error/oper/bool_expr_and_not_bool.sk: Warning: line 1 column 5: variable \"x\" should be constant\n" \
"./test/sh/error/oper/bool_expr_and_not_bool.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"mut x := 1 or 1\n",
TEST_DIR"/error/oper/bool_expr_or_not_bool.sk",

true,

"./test/sh/error/oper/bool_expr_or_not_bool.sk: Compilation error: line 1 column 10: expected type \"Bool\", got \"Int\"\n" \
"./test/sh/error/oper/bool_expr_or_not_bool.sk: Warning: line 1 column 5: variable \"x\" should be constant\n" \
"./test/sh/error/oper/bool_expr_or_not_bool.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"mut x := 1 xor 1\n",
TEST_DIR"/error/oper/bool_expr_xor_not_bool.sk",

true,

"./test/sh/error/oper/bool_expr_xor_not_bool.sk: Compilation error: line 1 column 10: expected type \"Bool\", got \"Int\"\n" \
"./test/sh/error/oper/bool_expr_xor_not_bool.sk: Warning: line 1 column 5: variable \"x\" should be constant\n" \
"./test/sh/error/oper/bool_expr_xor_not_bool.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 1 / 0\n",
TEST_DIR"/error/oper/div_by_zero.sk",

true,

"./test/sh/error/oper/div_by_zero.sk: Compilation error: line 1 column 10: division by zero\n" \
"./test/sh/error/oper/div_by_zero.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 / \"fail\"\n",
TEST_DIR"/error/oper/div_mismatched_consts.sk",

true,

"./test/sh/error/oper/div_mismatched_consts.sk: Compilation error: line 3 column 9: expected type \"Int\", got \"Str\"\n" \
"./test/sh/error/oper/div_mismatched_consts.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 0\n" \
"\n" \
"z := x + 1.0\n",
TEST_DIR"/error/oper/lhs_var_bad_type.sk",

true,

"./test/sh/error/oper/lhs_var_bad_type.sk: Compilation error: line 3 column 10: expected type \"Int\", got \"Float\"\n" \
"./test/sh/error/oper/lhs_var_bad_type.sk: Warning: line 1 column 1: variable \"x\" is unused\n" \
"./test/sh/error/oper/lhs_var_bad_type.sk: Warning: line 3 column 1: variable \"z\" is unused\n"
);


pass |= e2e_wrapper(
"x := fail + 1\n",
TEST_DIR"/error/oper/lhs_var_not_found.sk",

true,

"./test/sh/error/oper/lhs_var_not_found.sk: Compilation error: line 1 column 6: variable \"fail\" not found\n"
);


pass |= e2e_wrapper(
"x := x < 1\n",
TEST_DIR"/error/oper/lhs_var_self_ref.sk",

true,

"./test/sh/error/oper/lhs_var_self_ref.sk: Compilation error: line 1 column 6: variable \"x\" not found\n" \
"./test/sh/error/oper/lhs_var_self_ref.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 1 +\n",
TEST_DIR"/error/oper/missing_rhs.sk",

true,

"./test/sh/error/oper/missing_rhs.sk: Compilation error: line 1 column 8: expected expression after \"+\"\n"
);


pass |= e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 * \"fail\"\n",
TEST_DIR"/error/oper/mult_mismatched_consts.sk",

true,

"./test/sh/error/oper/mult_mismatched_consts.sk: Compilation error: line 3 column 9: expected type \"Int\", got \"Str\"\n" \
"./test/sh/error/oper/mult_mismatched_consts.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 0\n" \
"\n" \
"if x {\n" \
"	# do nothing\n" \
"}\n",
TEST_DIR"/error/oper/non_bool_cond.sk",

true,

"./test/sh/error/oper/non_bool_cond.sk: Compilation error: line 3 column 4: expected boolean expression\n" \
"./test/sh/error/oper/non_bool_cond.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := \"123\" ^ \"123\"\n",
TEST_DIR"/error/oper/pow_bad_type.sk",

true,

"./test/sh/error/oper/pow_bad_type.sk: Compilation error: cannot use type \"Str\" for power operator\n" \
"./test/sh/error/oper/pow_bad_type.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"mut x := 1\n" \
"\n" \
"x = 1.0\n",
TEST_DIR"/error/oper/reassign_different_type.sk",

true,

"./test/sh/error/oper/reassign_different_type.sk: Compilation error: line 3 column 5: expected type \"Int\", got \"Float\"\n" \
"./test/sh/error/oper/reassign_different_type.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 0\n" \
"\n" \
"z := 1.0 + x\n",
TEST_DIR"/error/oper/rhs_var_bad_type.sk",

true,

"./test/sh/error/oper/rhs_var_bad_type.sk: Compilation error: line 3 column 12: expected type \"Float\", got \"Int\"\n" \
"./test/sh/error/oper/rhs_var_bad_type.sk: Warning: line 1 column 1: variable \"x\" is unused\n" \
"./test/sh/error/oper/rhs_var_bad_type.sk: Warning: line 3 column 1: variable \"z\" is unused\n"
);


pass |= e2e_wrapper(
"x := 1 + fail\n",
TEST_DIR"/error/oper/rhs_var_not_found.sk",

true,

"./test/sh/error/oper/rhs_var_not_found.sk: Compilation error: line 1 column 10: variable \"fail\" not found\n" \
"./test/sh/error/oper/rhs_var_not_found.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"mut x := 0\n" \
"\n" \
"x = 1 - \"fail\"\n",
TEST_DIR"/error/oper/sub_mismatched_consts.sk",

true,

"./test/sh/error/oper/sub_mismatched_consts.sk: Compilation error: line 3 column 9: expected type \"Int\", got \"Str\"\n" \
"./test/sh/error/oper/sub_mismatched_consts.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"x := 1\n" \
"x\n",
TEST_DIR"/error/oper/trailing_token.sk",

true,

"./test/sh/error/oper/trailing_token.sk: Compilation error: line 2 column 1: expression cannot be used on its own\n" \
"./test/sh/error/oper/trailing_token.sk: Warning: line 1 column 1: variable \"x\" is unused\n"
);


pass |= e2e_wrapper(
"mut x := not 1\n",
TEST_DIR"/error/oper/using_non_bool_with_not.sk",

true,

"./test/sh/error/oper/using_non_bool_with_not.sk: Compilation error: line 1 column 14: expected type \"Bool\", got \"Int\"\n" \
"./test/sh/error/oper/using_non_bool_with_not.sk: Warning: line 1 column 5: variable \"x\" should be constant\n" \
"./test/sh/error/oper/using_non_bool_with_not.sk: Warning: line 1 column 5: variable \"x\" is unused\n"
);

	return pass;
}

static int e2e_wrapper(const char *code, const char *mock_file, bool error_expected, const char *expected) {
	char *output_file = NULL;

	if (error_expected) {
		output_file = mock_file_to_output_file(mock_file, ".out");
		BUILD_DATA.error_file = output_file;
	}
	else BUILD_DATA.error_file = "/dev/null";

	char *tmp = strdup(code);
	bool err = init_codegen_pipeline(mock_file, tmp);

	if (error_expected) {
		err = !err;
		err |= compare_compiler_output(output_file, expected);
		free(output_file);
	}
	else {
		output_file = mock_file_to_output_file(mock_file, ".ll");
		err |= compare_compiler_output(output_file, expected);
		free(output_file);
	}

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
		return false;
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
