pass=true

test() {
	echo -n "$1 "
	out=$(./build/src/skull ./test/sh/$1)

	if [ "$?" -ne "$2" ] || [ "$out" != "$3" ]; then
		/bin/echo -e "\033[91mFAIL\033[0m"
		pass=false
	else
		/bin/echo -e "\033[92mPASS\033[0m"
	fi
}

echo
echo "Running Skull unit tests"
echo

test "exit_code_0.sk" 0
test "exit_code_1.sk" 1
test "exit_code_variable.sk" 1
test "missing_file_extension_fails" 1 "missing required \".sk\" extension, exiting"
test "no_eol_works.sk" 0
test ".sk" 1 "\".sk\" is not a valid name, exiting"
test "not_a_file.sk" 1 "\"./test/sh/not_a_file.sk\" was not found, exiting"
test "option1\ option2" 1 "too many arguments passed, exiting"
test "print.sk" 0 "hello world!"

touch test/sh/read_protected.sk
chmod 200 test/sh/read_protected.sk
test "read_protected.sk" 1 "cannot open \"./test/sh/read_protected.sk\", permission denied"

echo

$pass || (echo "1 or more tests failed" && exit 1)