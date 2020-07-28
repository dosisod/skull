if [[ $(clang-tidy -checks=*,-llvm-header-guard,-readability-named-parameter $(find -L {skull,test} -type f | grep -E "\.(c|h)$") -header-filter=.* -quiet -- -std=c11 -I. 2>&1 | tee /dev/stderr | grep "skull") ]]; then
	exit 1
fi