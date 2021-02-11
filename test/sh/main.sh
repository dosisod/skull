#!/bin/sh

# shellcheck disable=SC2015,SC2044

passed=true

pass() {
	printf "\033[92mPASS\033[0m\n"
}

fail() {
	printf "\033[91mFAIL\033[0m\n"
	passed=false
}

compare() {
	cmp "$1" "$2" && pass || fail
}

test_normal() {
	dir=${1%/*}
	file=${1##*/_}
	file=${file%.*}

	printf "%s" "$dir/$file "

	rm -f "./$dir/.$file.ll"
	./build/skull/_skull "./$dir/$file"

	compare "./$dir/_$file.ll" "./$dir/.$file.ll"
	rm -f "./$dir/.$file.ll"
}

test_error() {
	dir=${1%/*}
	file=${1##*/_}
	file=${file%.*}

	printf "%s" "$dir/$file "

	./build/skull/_skull "./$dir/$file" 2> "./$dir/.$file.out"

	compare "./$dir/.$file.out" "./$dir/_$file.out"
	rm -f "./$dir/.$file.out"
}

test_option() {
	printf "%s" "$1 "

	[ "$(./build/skull/_skull "./test/sh/$1" 2>&1)" = "$2" ] && pass || fail
}

test_skull() {
	printf "%s" "skull $1 "

	# shellcheck disable=SC2086
	out=$(./build/skull/skull $2)

	sh -e "./test/sh/skull/$1" "$out" "$?"
	# shellcheck disable=SC2181
	[ "$?" != "0" ] && fail || pass
}

printf "\nRunning Skull unit tests\n\n"

for file in $(find test/sh/ -name "_*.ll") ; do
	test_normal "$file"
done

for file in $(find test/sh/error/ -name "_*.out") ; do
	test_error "$file"
done

touch test/sh/error/read_protected.sk
chmod 200 test/sh/error/read_protected.sk
printf "cannot open \"./test/sh/error/read_protected.sk\", permission denied\n" > test/sh/error/_read_protected.sk.out
test_error "test/sh/error/_read_protected.sk.out"
rm test/sh/error/_read_protected.sk.out
rm test/sh/error/read_protected.sk

test_option "missing_file_extension_fails" "missing required \".sk\" extension, exiting"
test_option ".sk" "\".sk\" is not a valid name, exiting"
test_option "not_a_file.sk" "\"./test/sh/not_a_file.sk\" was not found, exiting"

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

printf "\n"
$passed || (printf "1 or more tests failed\n" && exit 1)
