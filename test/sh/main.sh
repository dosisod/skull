#!/bin/sh

# shellcheck disable=SC2015,SC2044,SC2181

passed=true

pass() {
	printf "\033[92mPASS\033[0m\n"
}

fail() {
	printf "\033[91mFAIL\033[0m\n"
	passed=false
}

compare() {
	cmp "$1" "$2" > /dev/null 2>&1 && pass || fail
}

test_c_backend() {
	dir=${1%/*}
	file=${1##*/_}
	file=${file%.*}

	printf "%s" "$dir/$file "

	rm -f "./$dir/.$file.ll"
	C_BACKEND=1 ./build/skull/_skull "./$dir/$file" 2> /dev/null
	[ "$?" = "0" ] || { fail; return; }

	compare "./$dir/_$file.c" "./$dir/.$file.c"
	rm -f "./$dir/.$file.c"
}

test_llvm_debug() {
	dir=${1%/*}
	file=${1##*/_}
	file=${file%.*}

	printf "%s" "$dir/$file "

	rm -f "./$dir/.$file.ll"
	DEBUG=1 ./build/skull/_skull "./$dir/$file" 2> /dev/null
	[ "$?" = "0" ] || { fail; return; }

	sed -i "s/directory: \"\(.*\)\"/directory: \".\"/" "./$dir/.$file.ll"
	sed -i "s/producer: \"\(.*\)\"/producer: \"skull\"/" "./$dir/.$file.ll"
	compare "./$dir/_$file.ll" "./$dir/.$file.ll"
	rm -f "./$dir/.$file.ll"
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



find test/sh/ -name ".*.ll" -print0 | xargs -0 -I{} rm {}
find test/sh/ -name ".*.out" -print0 | xargs -0 -I{} rm {}
./build/test/e2e || passed=false
find test/sh/ -name ".*.ll" -print0 | xargs -0 -I{} rm {}
find test/sh/ -name ".*.out" -print0 | xargs -0 -I{} rm {}

for file in $(find test/skull/codegen/c/ -name "_*.c") ; do
	test_c_backend "$file"
done

for file in $(find test/skull/codegen/llvm/ -name "_*.ll") ; do
	test_llvm_debug "$file"
done

touch test/sh/error/read_protected.sk
chmod 200 test/sh/error/read_protected.sk
printf "skull: cannot open \"./test/sh/error/read_protected.sk\", permission denied\n" > test/sh/error/_read_protected.sk.out

./build/skull/_skull "./test/sh/error/read_protected.sk" 2> "./test/sh/error/.read_protected.sk.out"
[ "$?" = "1" ] || { fail; passed=false; }
printf "%s" "test/sh/error/read_protected.sk"

compare "./test/sh/error/.read_protected.sk.out" "./test/sh/error/_read_protected.sk.out"
rm -f "./test/sh/error/.read_protected.out"

rm test/sh/error/.read_protected.sk.out
rm test/sh/error/_read_protected.sk.out
rm test/sh/error/read_protected.sk

test_option "missing_file_extension_fails" "skull: missing required \".sk\" extension, exiting"
test_option ".sk" "skull: \".sk\" is not a valid name, exiting"
test_option "not_a_file.sk" "skull: \"./test/sh/not_a_file.sk\" was not found, exiting"

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
test_skull "directory_exists.sh" "./test/sh/skull/dir.sk"

printf "\n"
$passed || (printf "1 or more tests failed\n" && exit 1)
