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

	rm -f ./test/skullc/.$1.ll
	./build/skullc/_skullc ./test/skullc/$1

	[ "$(sha ./test/skullc/.$1.ll)" != "$(sha ./test/skullc/_$1.ll)" ]
	pass_or_fail $?

	rm -f ./test/skullc/.$1.ll
}

test_err() {
	echo -n "$1 "

	[ "$(./build/skullc/_skullc ./test/skullc/$1)" != "$2" ]
	pass_or_fail $?
}

echo
echo "Running Skullc unit tests"
echo

test "return_0.sk"
test "declare_int.sk"
test "declare_float.sk"
test "declare_bool.sk"
test "declare_str.sk"
test "declare_many.sk"
test "if_true.sk"
test "if_false.sk"
test "declare_mut.sk"
test "return_var.sk"
test "ignore_comments.sk"
test "no_eol_works.sk"

test_err "err_var_assign.sk" "Compilation error: variable \"x\" already defined"
test_err "err_return_int.sk" "Compilation error: overflow occurred while parsing \"0xffffffffffffffff\""
test_err "missing_file_extension_fails" "missing required \".sk\" extension, exiting"
test_err ".sk" "\".sk\" is not a valid name, exiting"
test_err "not_a_file.sk" "\"./test/skullc/not_a_file.sk\" was not found, exiting"
test_err "option1\ option2" "too many arguments passed, exiting"
test_err "illegal_utf8.sk" "illegal UTF8 sequence at character offset 0"

touch test/skullc/read_protected.sk
chmod 200 test/skullc/read_protected.sk
test_err "read_protected.sk" "cannot open \"./test/skullc/read_protected.sk\", permission denied"
rm test/skullc/read_protected.sk

echo

$pass || (echo "1 or more tests failed" && exit 1)