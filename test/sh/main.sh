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
	./build/skull/skull --c-backend "./$dir/$file" 2> /dev/null
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
	./build/skull/skull "./$dir/$file" -g -E -o "./$dir/.$file.ll" 2> /dev/null
	[ "$?" = "0" ] || { fail; return; }

	sed -i "s/directory: \"\(.*\)\"/directory: \".\"/" "./$dir/.$file.ll"
	sed -i "s/producer: \"\(.*\)\"/producer: \"skull\"/" "./$dir/.$file.ll"
	compare "./$dir/_$file.ll" "./$dir/.$file.ll"
	rm -f "./$dir/.$file.ll"
}

test_option() {
	printf "%s" "$1 "

	[ "$(./build/skull/skull "$1" 2>&1)" = "$2" ] && pass || fail
}

test_skull() {
	printf "%s" "skull $1 "

	# shellcheck disable=SC2086
	out=$(./build/skull/skull $2 2>&1)

	# shellcheck disable=SC2086
	sh -e ./test/sh/skull/$1 "$out" "$?"
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

./build/skull/skull "./test/sh/error/read_protected.sk" 2> "./test/sh/error/.read_protected.sk.out"
[ "$?" = "1" ] || { fail; passed=false; }
printf "%s " "test/sh/error/read_protected.sk"

compare "./test/sh/error/.read_protected.sk.out" "./test/sh/error/_read_protected.sk.out"
rm -f "./test/sh/error/.read_protected.out"

rm test/sh/error/.read_protected.sk.out
rm test/sh/error/_read_protected.sk.out
rm test/sh/error/read_protected.sk

test_option "missing_file_extension_fails" "skull: missing required \".sk\" extension, exiting"
test_option ".sk" "skull: \".sk\" is not a valid name, exiting"
test_option "not_a_file.sk" "skull: \"not_a_file.sk\" was not found, exiting"

test_skull "version.sh" "-v"
test_skull "version.sh" "--version"
test_skull "help.sh" "-h"
test_skull "help.sh" "--help"
test_skull "help.sh"
test_skull "bad_option.sh" "-z"
test_skull "bad_long_option.sh" "--invalid"
test_skull "preproc.sh" "./test/sh/skull/dummy.sk -E"
test_skull "asm.sh" "./test/sh/skull/dummy.sk -S"
test_skull "compile_obj.sh" "./test/sh/skull/dummy.sk -c"
test_skull "compile.sh" "./test/sh/skull/dummy.sk"
test_skull "compile_with_args.sh" "./test/sh/skull/dummy.sk -- -o ./test/sh/skull/alt_name"
test_skull "output_asm.sh" "./test/sh/skull/dummy.sk -S -o test/sh/skull/alt_name"
test_skull "output_obj.sh" "./test/sh/skull/dummy.sk -c -o test/sh/skull/alt_name"
test_skull "directory_exists.sh" "./test/sh/skull/dir.sk"
test_skull "no_file_passed.sh" "-E"
test_skull "duplicate_arg.sh -E" "-E -E"
test_skull "duplicate_arg.sh -c" "-c -c"
test_skull "duplicate_arg.sh -S" "-S -S"
test_skull "duplicate_arg.sh -q" "-q -q"
test_skull "duplicate_arg.sh -g" "-g -g"
test_skull "duplicate_arg.sh -O1" "-O1 -O1"
test_skull "duplicate_arg.sh -O2" "-O2 -O2"
test_skull "duplicate_arg.sh -O3" "-O3 -O3"
test_skull "duplicate_arg.sh --c-backend" "--c-backend --c-backend"
test_skull "duplicate_arg.sh --werror" "--werror --werror"
test_skull "warn_dash_dash_no_args.sh" "--"
test_skull "dash_o_expect_filename.sh" "-o"
test_skull "dash_o_no_binary.sh" "./test/sh/skull/dummy.sk -o -"
test_skull "werror.sh" "./test/sh/skull/werror.sk --werror"
test_skull "invalid_short_option.sh" "-xy"
test_skull "c_backend_dash_o.sh" "./test/sh/skull/dummy.sk --c-backend -o -"
test_skull "trailing_arg_after_dash_o.sh" "-o ./test/sh/skull/something ./test/sh/skull/dummy.sk"
test_skull "optimization.sh" "./test/sh/skull/o1.sk -O1 -E"
test_skull "invalid_optimization_flag.sh" "-Ox"

printf "\n"
$passed || (printf "1 or more tests failed\n" && exit 1)
