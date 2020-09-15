[[ ! $(clang-tidy-9 \
	$(find -L {skull*,test} -type f | grep -E "\.(c|h)$") \
	-checks=*,-llvm-header-guard,-readability-named-parameter,-readability-avoid-const-params-in-decls,-google-readability-avoid-underscore-in-googletest-name,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling \
	-header-filter=.* \
	-quiet -- \
	-std=c17 -I. $(llvm-config-9 --cflags | awk '{print $1}') 2>&1 | tee /dev/stderr | grep "skull"
) ]]