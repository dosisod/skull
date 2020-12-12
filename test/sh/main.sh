#!/bin/sh

pass=true

sha() {
	echo $(sha512sum $1 | awk '{print $1}')
}

cmp() {
	sha_control=$(sha $1)
	sha_test=$(sha $2)
	[ "$sha_test" != "$sha_control" ] || [ -z "$sha_test" ]
	pass_or_fail $?
}

fail() {
	/bin/echo -e "\033[91mFAIL\033[0m"
	pass=false
}

pass() {
	/bin/echo -e "\033[92mPASS\033[0m"
}

pass_or_fail() {
	[ "$1" = "0" ] && fail || pass
}

test() {
	echo -n "$1 $2 "

	rm -f ./test/sh/$1/.$2.ll
	./build/skull/_skull ./test/sh/$1/$2

	cmp ./test/sh/$1/_$2.ll ./test/sh/$1/.$2.ll

	rm -f ./test/sh/$1/.$2.ll
}

test_option() {
	echo -n "$1 "

	[ "$(./build/skull/_skull ./test/sh/$1)" != "$2" ]
	pass_or_fail $?
}

test_error() {
	echo -n "error $1 "

	./build/skull/_skull ./test/sh/error/$1 > ./test/sh/error/.$1.out

	cmp ./test/sh/error/.$1.out ./test/sh/error/_$1.out

	rm -f ./test/sh/error/.$1.out
}

test_skull() {
	echo -n "skull $1 "

	out=$(./build/skull/skull $2)

	sh ./test/sh/skull/$1 "$out" "$?"
	[ "$?" != "0" ]
	pass_or_fail $?
}

echo
echo "Running Skull unit tests"
echo

test "declare" "bool.sk"
test "declare" "float_infinity.sk"
test "declare" "float_negative_infinity.sk"
test "declare" "float_negative.sk"
test "declare" "float.sk"
test "declare" "float_zero.sk"
test "declare" "int_binary.sk"
test "declare" "int_hex.sk"
test "declare" "int_negative.sk"
test "declare" "int_octal.sk"
test "declare" "int.sk"
test "declare" "many.sk"
test "declare" "mut_explicit.sk"
test "declare" "mut.sk"
test "declare" "rune_escaped.sk"
test "declare" "rune_null.sk"
test "declare" "rune.sk"
test "declare" "str.sk"
test "declare" "escape_sequences.sk"
test "declare" "variable_auto.sk"
test "declare" "reassign_str.sk"
test "declare" "reassign_var.sk"

test "oper" "add_float_consts.sk"
test "oper" "add_int_consts.sk"
test "oper" "mult_float_consts.sk"
test "oper" "mult_int_consts.sk"
test "oper" "sub_float_consts.sk"
test "oper" "sub_int_consts.sk"
test "oper" "div_float_consts.sk"
test "oper" "div_int_consts.sk"
test "oper" "lhs_with_var.sk"
test "oper" "rhs_with_var.sk"

test "function" "assign_return_value.sk"
test "function" "call_str.sk"
test "function" "single_param.sk"
test "function" "var_param.sk"
test "function" "external_return_type.sk"
test "function" "external_with_param.sk"
test "function" "multiple_externals.sk"
test "function" "nodes_after_call_not_ignored.sk"
test "function" "simple.sk"
test "function" "declare_func.sk"
test "function" "func_with_scope.sk"
test "function" "multi_var_func.sk"
test "function" "nested_if.sk"
test "function" "exported.sk"
test "function" "non_int_return.sk"

test "flow" "if_empty_body.sk"
test "flow" "if_false.sk"
test "flow" "if_true.sk"
test "flow" "if_with_var_false.sk"
test "flow" "if_with_var_true.sk"
test "flow" "if_else_comment.sk"
test "flow" "else.sk"
test "flow" "set_var_in_if.sk"
test "flow" "return_0.sk"
test "flow" "return_var.sk"
test "flow" "while.sk"
test "flow" "nested_if.sk"

test "misc" "const_inline.sk"
test "misc" "ignore_comments.sk"
test "misc" "ignore_block_comments.sk"
test "misc" "no_eol_works.sk"
test "misc" "locale.sk"
test "misc" "comment_then_code.sk"

test_option "missing_file_extension_fails" "missing required \".sk\" extension, exiting"
test_option ".sk" "\".sk\" is not a valid name, exiting"
test_option "not_a_file.sk" "\"./test/sh/not_a_file.sk\" was not found, exiting"
test_option "option1\ option2\ option3" "unexpected number of parameters"

test_error "var_already_defined.sk"
test_error "return_int_overflow.sk"
test_error "declare_int_overflow.sk"
test_error "declare_int_underflow.sk"
test_error "declare_float_overflow.sk"
test_error "illegal_utf8.sk"
test_error "assign_missing_rhs.sk"
test_error "auto_assign_missing_rhs.sk"
test_error "assign_unknown_var.sk"
test_error "assign_unassignable_expression.sk"
test_error "return_non_existent_var.sk"
test_error "return_non_int.sk"
test_error "mismatched_bracket.sk"
test_error "stray_close_bracket.sk"
test_error "extra_close_bracket.sk"
test_error "no_closing_bracket.sk"
test_error "invalid_return_if.sk"
test_error "if_with_nonexisting_var.sk"
test_error "only_identifier_fails.sk"
test_error "if_var_out_of_scope.sk"
test_error "if_missing_block.sk"
test_error "reassign_non_existet_var.sk"
test_error "reassign_const_var.sk"
test_error "reassign_missing_rhs.sk"
test_error "unexpected_str_fails.sk"
test_error "add_mismatched_consts.sk"
test_error "sub_mismatched_consts.sk"
test_error "mult_mismatched_consts.sk"
test_error "div_mismatched_consts.sk"
test_error "div_by_zero.sk"
test_error "assign_mismatch_var_type.sk"
test_error "missing_external.sk"
test_error "redeclare_external.sk"
test_error "no_newline_external.sk"
test_error "no_content.sk"
test_error "escape_missing_control.sk"
test_error "escape_hex_missing_both.sk"
test_error "escape_hex_missing_second.sk"
test_error "escape_hex_first_bad.sk"
test_error "escape_hex_second_bad.sk"
test_error "function_bad_param_type.sk"
test_error "function_param_unknown_var.sk"
test_error "function_param_bad_type.sk"
test_error "function_shadowed_param.sk"
test_error "assign_void_function.sk"
test_error "function_missing_return.sk"
test_error "function_unexpected_return.sk"
test_error "function_missing_declaration.sk"
test_error "oper_rhs_var_not_found.sk"
test_error "oper_rhs_var_bad_type.sk"
test_error "oper_lhs_var_not_found.sk"
test_error "oper_lhs_var_bad_type.sk"
test_error "redundant_assign.sk"
test_error "closing_block_comment_missing.sk"
test_error "unexpected_code_block.sk"
test_error "else_without_if.sk"
test_error "unreachable_return.sk"
test_error "invalid_identifier.sk"
test_error "main_reserved.sk"
test_error "non_int_return.sk"

touch test/sh/error/read_protected.sk
chmod 200 test/sh/error/read_protected.sk
test_error "read_protected.sk"
rm test/sh/error/read_protected.sk

test_skull "version.sh" "-v"
test_skull "help.sh" "-h"
test_skull "help.sh"
test_skull "bad_option.sh" "-z"
test_skull "preproc.sh" "./test/sh/skull/dummy.sk -E"
test_skull "asm.sh" "./test/sh/skull/dummy.sk -S"
test_skull "compile_obj.sh" "./test/sh/skull/dummy.sk -c"
test_skull "compile.sh" "./test/sh/skull/dummy.sk"
test_skull "compile_with_args.sh" "./test/sh/skull/dummy.sk -- -o ./test/sh/skull/alt_name"
test_skull "output_asm.sh" "./test/sh/skull/dummy.sk -S -o test/sh/skull/alt_name"
test_skull "output_obj.sh" "./test/sh/skull/dummy.sk -c -o test/sh/skull/alt_name"
test_skull "rename_main.sh" "./test/sh/skull/dummy.sk -m alt_main -E"

echo
$pass || (echo "1 or more tests failed" && exit 1)