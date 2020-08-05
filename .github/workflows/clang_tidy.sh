[[ ! $(clang-tidy-9 \
	$(find -L {skull,test} -type f | grep -E "\.(c|h)$") \
	-checks=*,-llvm-header-guard,-readability-named-parameter,-google-readability-avoid-underscore-in-googletest-name,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling \
	-header-filter=.* \
	-quiet -- \
	-std=c17 -I. 2>&1 | tee /dev/stderr | grep "skull"
) ]]