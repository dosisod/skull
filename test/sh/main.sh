pass=true

test() {
	echo -n "$1 "
	./build/src/skull ./test/sh/$1 > /dev/null
	if [ "$?" -ne "$2" ]; then
		/bin/echo -e "\033[91mFAIL\033[0m\n"
		pass=false
	else
		/bin/echo -e "\033[92mPASS\033[0m"
	fi
}

echo "Running Skull unit tests"

test "exit_code_0.sk" 0
test "exit_code_1.sk" 1
test "exit_code_variable.sk" 1
test "missing_file_extension_fails" 1
test "no_eol_works.sk" 0

$pass || (echo "1 or more tests failed" && exit 1)