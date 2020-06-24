if [[ $(clang-tidy -checks=*,-llvm-header-guard src/**/*.h -quiet -- -std=c11 2>&1 | tee /dev/stderr | grep "skull") ]]; then
	exit 1
fi

if [[ $(clang-tidy -checks=*,-llvm-header-guard test/**/*.h -quiet -- -std=c11 2>&1 | tee /dev/stderr | grep "skull") ]]; then
	exit 1
fi