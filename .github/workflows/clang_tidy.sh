CLANG_TIDY=$(command -v clang-tidy-10 || command -v clang-tidy)
LLVM_CONFIG=$(command -v llvm-config-10 || command -v llvm-config)

[ ! $($CLANG_TIDY \
	$(find -L skull test -type f | grep -E "\.(c|h)$") \
	-checks=*,-hicpp-signed-bitwise,-llvm-header-guard,-readability-named-parameter,-readability-avoid-const-params-in-decls,-google-readability-avoid-underscore-in-googletest-name,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cppcoreguidelines-init-variables \
	-header-filter=.* \
	-quiet -- \
	-std=c17 -I. $($LLVM_CONFIG --cflags) 2>&1 | tee /dev/stderr | grep "skull"
) ]