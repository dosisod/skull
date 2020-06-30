if [[ $(clang-tidy -checks=*,-llvm-header-guard $(find -L {src,test} -type f | grep -E "\.(c|h)$") -quiet -- -std=c11 2>&1 | tee /dev/stderr | grep "skull") ]]; then
	exit 1
fi