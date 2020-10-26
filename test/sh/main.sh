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

test_err() {
	echo -n "$1 "

	[ "$(./build/skull/_skull ./test/sh/$1)" != "$2" ]
	pass_or_fail $?
}

echo
echo "Running Skull unit tests"
echo

test "return_0.sk"
test "declare_int.sk"
test "declare_float.sk"
test "declare_float_zero.sk"
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
test "simple_function_call.sk"
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
test "function_single_param.sk"

test_err "missing_file_extension_fails" "missing required \".sk\" extension, exiting"
test_err ".sk" "\".sk\" is not a valid name, exiting"
test_err "not_a_file.sk" "\"./test/sh/not_a_file.sk\" was not found, exiting"
test_err "option1\ option2" "too many arguments passed, exiting"

test_err "error/var_already_defined.sk" "Compilation error: variable \"x\" already defined"
test_err "error/return_int_overflow.sk" "Compilation error: overflow occurred while parsing \"0xffffffffffffffff\""
test_err "error/declare_int_overflow.sk" "Compilation error: overflow occurred while parsing \"99999999999999999999999999999999\""
test_err "error/illegal_utf8.sk" "illegal UTF8 sequence at character offset 0"
test_err "error/assign_missing_rhs.sk" "Compilation error: missing value in assignment to variable \"x\""
test_err "error/auto_assign_missing_rhs.sk" "Compilation error: missing value in assignment to variable \"x\""
test_err "error/assign_unknown_var.sk" "Compilation error: variable \"y\" not found"
test_err "error/return_non_existent_var.sk" "Compilation error: variable \"x\" not found"
test_err "error/return_non_int.sk" "Compilation error: returning non-int variable \"x\" from main"
test_err "error/mismatched_bracket.sk" "Compilation error: Reached EOF, expected closing bracket"
test_err "error/stray_close_bracket.sk" "Compilation error: unexpected token: \"}\""
test_err "error/extra_close_bracket.sk" "Compilation error: unexpected token: \"}\""
test_err "error/no_closing_bracket.sk" "Compilation error: Reached EOF, expected closing bracket"
test_err "error/invalid_return_if.sk" "Compilation error: unexpected token: \"x\""
test_err "error/if_with_nonexisting_var.sk" "Compilation error: variable \"x\" not found"
test_err "error/only_identifier_fails.sk" "Compilation error: unexpected token: \"x\""
test_err "error/reassign_non_existet_var.sk" "Compilation error: variable \"x\" not found"
test_err "error/reassign_const_var.sk" "Compilation error: cannot reassign const variable \"x\""
test_err "error/reassign_missing_rhs.sk" "Compilation error: missing value in assignment to variable \"x\""
test_err "error/unexpected_str_fails.sk" "Compilation error: unexpected token: \"\"this will fail\"\""
test_err "error/add_mismatched_consts.sk" "Compilation error: cannot add \"1\" and \"\"fail\"\""
test_err "error/sub_mismatched_consts.sk" "Compilation error: cannot subtract \"1\" and \"\"fail\"\""
test_err "error/mult_mismatched_consts.sk" "Compilation error: cannot multiply \"1\" and \"\"fail\"\""
test_err "error/div_mismatched_consts.sk" "Compilation error: cannot divide \"1\" and \"\"fail\"\""
test_err "error/assign_mismatch_var_type.sk" "Compilation error: type mismatch: expected type \"bool\""
test_err "error/missing_external.sk" "Compilation error: external function \"x\" missing external declaration"
test_err "error/redeclare_external.sk" "Compilation error: cannot redeclare external function \"x\""

touch test/sh/error/read_protected.sk
chmod 200 test/sh/error/read_protected.sk
test_err "error/read_protected.sk" "cannot open \"./test/sh/error/read_protected.sk\", permission denied"
rm test/sh/error/read_protected.sk

echo

$pass || (echo "1 or more tests failed" && exit 1)