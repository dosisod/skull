pass=true

sha() {
	echo $(sha512sum $1 | awk '{print $1}')
}

test() {
	echo -n "$1 "

	rm -f ./test/skullc/.$1.ll
	./build/skullc/_skullc ./test/skullc/$1

	if [ "$(sha ./test/skullc/.$1.ll)" != "$(sha ./test/skullc/_$1.ll)" ]; then
		/bin/echo -e "\033[91mFAIL\033[0m"
		pass=false
	else
		/bin/echo -e "\033[92mPASS\033[0m"
	fi

	rm -f ./test/skullc/.$1.ll
}

echo
echo "Running Skullc unit tests"
echo

test "return_0.sk"
test "declare_int.sk"
test "declare_float.sk"
test "declare_bool.sk"
test "declare_str.sk"

echo

$pass || (echo "1 or more tests failed" && exit 1)