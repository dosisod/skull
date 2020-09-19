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
test "declare_bool.sk"
test "declare_str.sk"
test "declare_rune.sk"
test "declare_many.sk"
test "if_true.sk"
test "if_false.sk"
test "declare_mut.sk"
test "declare_mut_explicit.sk"
test "return_var.sk"
test "ignore_comments.sk"
test "no_eol_works.sk"
test "assign_variable_auto.sk"

test_err "err_var_assign.sk" "Compilation error: variable \"x\" already defined"
test_err "err_return_int.sk" "Compilation error: overflow occurred while parsing \"0xffffffffffffffff\""
test_err "missing_file_extension_fails" "missing required \".sk\" extension, exiting"
test_err ".sk" "\".sk\" is not a valid name, exiting"
test_err "not_a_file.sk" "\"./test/sh/not_a_file.sk\" was not found, exiting"
test_err "option1\ option2" "too many arguments passed, exiting"
test_err "illegal_utf8.sk" "illegal UTF8 sequence at character offset 0"
test_err "redeclare_var.sk" "Compilation error: variable \"x\" already defined"
test_err "assign_missing_rhs.sk" "Compilation error: missing value in assignment to variable \"x\""
test_err "auto_assign_missing_rhs.sk" "Compilation error: missing value in assignment to variable \"x\""
test_err "assign_unknown_var.sk" "Compilation error: variable \"y\" not found"
test_err "return_non_existent_var.sk" "Compilation error: variable \"x\" not found"
test_err "return_non_int.sk" "Compilation error: returning non-int variable \"x\" from main"
test_err "return_non_int.sk" "Compilation error: returning non-int variable \"x\" from main"
test_err "if_missing_body.sk" "Compilation error: unexpected token: \"if\""
test_err "err_mismatched_bracket.sk" "Compilation error: unexpected token: \"[\""
test_err "err_stray_close_bracket.sk" "Compilation error: unexpected token: \"]\""
test_err "err_extra_close_bracket.sk" "Compilation error: unexpected token: \"]\""

touch test/sh/read_protected.sk
chmod 200 test/sh/read_protected.sk
test_err "read_protected.sk" "cannot open \"./test/sh/read_protected.sk\", permission denied"
rm test/sh/read_protected.sk

echo

$pass || (echo "1 or more tests failed" && exit 1)