[[ ! $(clang-tidy \
	$(find -L {skull,test} -type f | grep -E "\.(c|h)$") \
	-checks=*,-llvm-header-guard,-readability-named-parameter \
	-header-filter=.* \
	-quiet -- \
	-std=c17 -I. $(llvm-config-9 --cflags | awk '{print $1}') 2>&1 | tee /dev/stderr | grep "skull"
) ]]