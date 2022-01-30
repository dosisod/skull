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

#include "test/sh/e2e_inner.h"

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

"./test/sh/error/func/proto_param_bad_type.sk: Compilation error: line 1 column 6: type \"does_not_exist\" could not be found\n"
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

"./test/sh/error/misc/invalid_paren_expr.sk: Compilation error: line 1 column 6: invalid expression near \"(\"\n"
);


pass |= e2e_wrapper(
"x := (",
TEST_DIR"/error/misc/invalid_paren_expr2.sk",

NULL,

"./test/sh/error/misc/invalid_paren_expr2.sk: Compilation error: line 1 column 6: invalid expression near \"(\"\n"
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
"external x() x()",
TEST_DIR"/error/misc/no_newline_external.sk",

NULL,

"./test/sh/error/misc/no_newline_external.sk: Compilation error: line 1 column 14: expected a newline\n"
);


pass |= e2e_wrapper(
"{ noop }\n",
TEST_DIR"/error/misc/only_brackets.sk",

NULL,

"./test/sh/error/misc/only_brackets.sk: Compilation error: line 1 column 3: unexpected code block\n"
);


pass |= e2e_wrapper(
"x := (1",
TEST_DIR"/error/misc/paren_expr_missing_paren.sk",

NULL,

"./test/sh/error/misc/paren_expr_missing_paren.sk: Compilation error: line 1 column 6: missing closing parenthesis\n"
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

pass |= e2e_wrapper(
"x := &1",
TEST_DIR"/error/oper/check_non_ident_ref.sk",

NULL,

"./test/sh/error/oper/check_non_ident_ref.sk: Compilation error: line 1 column 7: cannot take reference of non-identifier\n"
);


pass |= e2e_wrapper(
"f() { noop }\n"\
"x: Void = f()",
TEST_DIR"/error/func/no_explicit_void_assign.sk",

NULL,

"./test/sh/error/func/no_explicit_void_assign.sk: Compilation error: line 2 column 11: function returning type void cannot be assigned to variable \"x\"\n"
);


pass |= e2e_wrapper(
"x: &invalid_type = 0",
TEST_DIR"/error/declare/invalid_explicit_ref.sk",

NULL,

"./test/sh/error/declare/invalid_explicit_ref.sk: Compilation error: line 1 column 4: type \"&\" could not be found\n"
);


pass |= e2e_wrapper(
"x: y",
TEST_DIR"/error/declare/no_assignment.sk",

NULL,

"./test/sh/error/declare/no_assignment.sk: Compilation error: line 1 column 1: variable is missing an assignment\n"
);


pass |= e2e_wrapper(
"f(",
TEST_DIR"/error/func/no_closing_paren.sk",

NULL,

"./test/sh/error/func/no_closing_paren.sk: Compilation error: line 1 column 2: missing closing parenthesis\n"
);


pass |= e2e_wrapper(
"f(x: Int",
TEST_DIR"/error/func/no_closing_paren_single_arg.sk",

NULL,

"./test/sh/error/func/no_closing_paren_single_arg.sk: Compilation error: line 1 column 2: missing closing parenthesis\n"
);


pass |= e2e_wrapper(
"1 +",
TEST_DIR"/error/oper/rhs_missing.sk",

NULL,

"./test/sh/error/oper/rhs_missing.sk: Compilation error: line 1 column 3: expected expression after \"+\"\n"
);


pass |= e2e_wrapper(
"f(1",
TEST_DIR"/error/func/single_expr_call.sk",

NULL,

"./test/sh/error/func/single_expr_call.sk: Compilation error: line 1 column 3: missing closing parenthesis\n"
);


pass |= e2e_wrapper(
"f() Int",
TEST_DIR"/error/func/missing_body.sk",

NULL,

"./test/sh/error/func/missing_body.sk: Compilation error: line 1 column 5: missing opening bracket\n"
);


pass |= e2e_wrapper(
"-",
TEST_DIR"/error/expr/unary_missing_rhs.sk",

NULL,

"./test/sh/error/expr/unary_missing_rhs.sk: Compilation error: line 1 column 1: expected expression after \"-\"\n"
);


pass |= e2e_wrapper(
"f() { noop }\n" \
"\n" \
"f(,)",

TEST_DIR"/error/expr/func_call_extra_comma.sk",

NULL,

"./test/sh/error/expr/func_call_extra_comma.sk: Compilation error: line 3 column 3: unexpected comma\n"
);


pass |= e2e_wrapper(
"x := * 1\n",

TEST_DIR"/error/expr/deref_constant_fails.sk",

NULL,

"./test/sh/error/expr/deref_constant_fails.sk: Compilation error: line 1 column 8: cannot dereference expression\n"
);


pass |= e2e_wrapper(
"a := 1\n" \
"b := * a\n",

TEST_DIR"/error/expr/deref_non_ref_fails.sk",

NULL,

"./test/sh/error/expr/deref_non_ref_fails.sk: Compilation error: line 2 column 8: cannot dereference expression\n"
);


pass |= e2e_wrapper(
"x := true not true\n",

TEST_DIR"/error/expr/unexpected_unary.sk",

NULL,

"./test/sh/error/expr/unexpected_unary.sk: Compilation error: line 1 column 11: unexpected unary operator\n"
);


pass |= e2e_wrapper(
"mut x := 1\n" \
"x = x\n",

TEST_DIR"/error/declare/redundant_reassign.sk",

NULL,

"./test/sh/error/declare/redundant_reassign.sk: Compilation error: line 2 column 5: redundant assignment of variable \"x\" to itself\n"
);


pass |= e2e_wrapper(
"import",

TEST_DIR"/error/flow/import_missing_token.sk",

NULL,

"./test/sh/error/flow/import_missing_token.sk: Compilation error: line 1 column 1: expected identifier after \"import\"\n"
);


pass |= e2e_wrapper(
"import 1",

TEST_DIR"/error/flow/import_missing_ident.sk",

NULL,

"./test/sh/error/flow/import_missing_ident.sk: Compilation error: line 1 column 8: expected identifier after \"import\"\n"
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
		fprintf(stderr, "%s " COLOR_BOLD COLOR_RED_FG "FAIL" COLOR_RESET "\n", mock_file);
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
