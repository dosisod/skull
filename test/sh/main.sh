#!/usr/bin/env bash

pass=true

sha() {
	echo $(sha512sum $1 | awk '{print $1}')
}

pass_or_fail() {
	if [ "$1" == "0" ]; then
		/bin/echo -e "\033[91mFAIL\033[0m"
		pass=false
	else
		/bin/echo -e "\033[92mPASS\033[0m"
	fi
}

test() {
	echo -n "$1 "

	rm -f ./test/sh/.$1.ll
	./build/skull/_skull ./test/sh/$1

	[ "$(sha ./test/sh/.$1.ll)" != "$(sha ./test/sh/_$1.ll)" ]
	pass_or_fail $?

	rm -f ./test/sh/.$1.ll
}

test_option() {
	echo -n "$1 "

	[ "$(./build/skull/_skull ./test/sh/$1)" != "$2" ]
	pass_or_fail $?
}

test_error() {
	echo -n "error/$1 "

	./build/skull/_skull ./test/sh/error/$1 > ./test/sh/error/.$1.out

	[ "$(sha ./test/sh/error/.$1.out)" != "$(sha ./test/sh/error/_$1.out)" ]
	pass_or_fail $?

	rm -f ./test/sh/error/.$1.out
}

echo
echo "Running Skull unit tests"
echo

test "return_0.sk"
test "declare_int.sk"
test "declare_int_negative.sk"
test "declare_int_hex.sk"
test "declare_int_octal.sk"
test "declare_int_binary.sk"
test "declare_float.sk"
test "declare_float_zero.sk"
test "declare_float_negative.sk"
test "declare_float_infinity.sk"
test "declare_float_negative_infinity.sk"
test "declare_bool.sk"
test "declare_str.sk"
test "declare_rune.sk"
test "declare_rune_escaped.sk"
test "declare_many.sk"
test "if_true.sk"
test "if_false.sk"
test "declare_mut.sk"
test "declare_mut_explicit.sk"
test "return_var.sk"
test "ignore_comments.sk"
test "no_eol_works.sk"
test "assign_variable_auto.sk"
test "assign_function_return.sk"
test "simple_function_call.sk"
test "function_call_str.sk"
test "nodes_after_call_not_ignored.sk"
test "set_var_in_if.sk"
test "if_with_var_true.sk"
test "if_with_var_false.sk"
test "reassign_var.sk"
test "if_empty_body.sk"
test "reassign_str.sk"
test "add_int_consts.sk"
test "add_float_consts.sk"
test "sub_int_consts.sk"
test "sub_float_consts.sk"
test "mult_int_consts.sk"
test "mult_float_consts.sk"
test "div_int_consts.sk"
test "div_float_consts.sk"
test "multiple_externals.sk"
test "external_with_param.sk"
test "external_return_type.sk"
test "function_single_param.sk"
test "function_var_param.sk"
test "escape_sequences.sk"
test "oper_rhs_with_var.sk"
test "oper_lhs_with_var.sk"

test_option "missing_file_extension_fails" "missing required \".sk\" extension, exiting"
test_option ".sk" "\".sk\" is not a valid name, exiting"
test_option "not_a_file.sk" "\"./test/sh/not_a_file.sk\" was not found, exiting"
test_option "option1\ option2" "too many arguments passed, exiting"

test_error "var_already_defined.sk"
test_error "return_int_overflow.sk"
test_error "declare_int_overflow.sk"
test_error "declare_int_underflow.sk"
test_error "declare_float_overflow.sk"
test_error "illegal_utf8.sk"
test_error "assign_missing_rhs.sk"
test_error "auto_assign_missing_rhs.sk"
test_error "assign_unknown_var.sk"
test_error "return_non_existent_var.sk"
test_error "return_non_int.sk"
test_error "mismatched_bracket.sk"
test_error "stray_close_bracket.sk"
test_error "extra_close_bracket.sk"
test_error "no_closing_bracket.sk"
test_error "invalid_return_if.sk"
test_error "if_with_nonexisting_var.sk"
test_error "only_identifier_fails.sk"
test_error "reassign_non_existet_var.sk"
test_error "reassign_const_var.sk"
test_error "reassign_missing_rhs.sk"
test_error "unexpected_str_fails.sk"
test_error "add_mismatched_consts.sk"
test_error "sub_mismatched_consts.sk"
test_error "mult_mismatched_consts.sk"
test_error "div_mismatched_consts.sk"
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
test_error "oper_rhs_var_not_found.sk"
test_error "oper_rhs_var_bad_type.sk"
test_error "oper_lhs_var_not_found.sk"
test_error "oper_lhs_var_bad_type.sk"

touch test/sh/error/read_protected.sk
chmod 200 test/sh/error/read_protected.sk
test_error "read_protected.sk"
rm test/sh/error/read_protected.sk

echo

$pass || (echo "1 or more tests failed" && exit 1)