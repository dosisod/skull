CLANG_TIDY=$(command -v clang-tidy-9 || command -v clang-tidy)

[ ! $($CLANG_TIDY \
	$(find -L skull test -type f | grep -E "\.(c|h)$") \
	-checks=*,-hicpp-signed-bitwise,-llvm-header-guard,-readability-named-parameter,-readability-avoid-const-params-in-decls,-google-readability-avoid-underscore-in-googletest-name,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers \
	-header-filter=.* \
	-quiet -- \
	-std=c17 -I. $(llvm-config-9 --cflags) 2>&1 | tee /dev/stderr | grep "skull"
) ]