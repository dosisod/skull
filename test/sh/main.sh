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

test_normal() {
	dir=$(dirname "$1")
	file=$(basename "$1")
	file="$(expr substr "${file%.*}" 2 99999)"

	echo -n "$dir/$file "

	rm -f "./$dir/.$file.ll"
	./build/skull/_skull "./$dir/$file"

	cmp "./$dir/_$file.ll" "./$dir/.$file.ll"
	rm -f "./$dir/.$file.ll"
}

test_error() {
	dir=$(dirname "$1")
	file=$(basename "$1")
	file="$(expr substr "${file%.*}" 2 99999)"

	echo -n "$dir/$file "

	./build/skull/_skull "./test/sh/error/$file" > "./test/sh/error/.$file.out"

	cmp "./test/sh/error/.$file.out" "./test/sh/error/_$file.out"
	rm -f "./test/sh/error/.$file.out"
}

test_option() {
	echo -n "$1 "

	[ "$(./build/skull/_skull ./test/sh/$1)" != "$2" ]
	pass_or_fail $?
}

test_skull() {
	echo -n "skull $1 "

	out=$(./build/skull/skull $2)

	sh -e "./test/sh/skull/$1" "$out" "$?"
	[ "$?" != "0" ]
	pass_or_fail $?
}

echo
echo "Running Skull unit tests"
echo

for file in $(find test/sh/ -name "_*.ll") ; do
	test_normal "$file"
done

for file in $(find test/sh/error/ -name "_*.out") ; do
	test_error "$file"
done

touch test/sh/error/read_protected.sk
chmod 200 test/sh/error/read_protected.sk
echo "cannot open \"./test/sh/error/read_protected.sk\", permission denied" > test/sh/error/_read_protected.sk.out
test_error "test/sh/error/_read_protected.sk.out"
rm test/sh/error/_read_protected.sk.out
rm test/sh/error/read_protected.sk

test_option "missing_file_extension_fails" "missing required \".sk\" extension, exiting"
test_option ".sk" "\".sk\" is not a valid name, exiting"
test_option "not_a_file.sk" "\"./test/sh/not_a_file.sk\" was not found, exiting"
test_option "option1\ option2\ option3" "unexpected number of parameters"

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