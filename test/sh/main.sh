pass=true

test() {
	echo -n "$1 "
	./build/src/skull ./test/sh/$1.skull
	if [ "$?" -ne "$2" ]; then
		/bin/echo -e "\033[91mFAIL\033[0m\n"
		pass=false
	else
		/bin/echo -e "\033[92mPASS\033[0m"
	fi
}

echo "Running Skull unit tests"

test "exit_code_0" 0
test "exit_code_1" 1
test "exit_code_variable" 1

$pass || (echo "1 or more tests failed" && exit 1)