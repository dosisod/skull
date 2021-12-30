#!/bin/sh

CLANG_TIDY=$(command -v clang-tidy-13 || command -v clang-tidy || command -v clang-tidy-12)
LLVM_CONFIG=$(command -v llvm-config-13 || command -v llvm-config || command -v llvm-config-12)

[ ! "$($CLANG_TIDY \
	$(find -L skull test -type f -name "*.[ch]" ! -name "*.sk.c" ! -path "*/sh/*") \
	-checks=*,-google-readability-braces-around-statements,-readability-braces-around-statements,\
-hicpp-braces-around-statements,-hicpp-signed-bitwise,-llvm-header-guard,\
-readability-named-parameter,-readability-avoid-const-params-in-decls,\
-google-readability-avoid-underscore-in-googletest-name,\
-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,\
-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,\
-cppcoreguidelines-init-variables,\
-llvmlibc-restrict-system-libc-headers,-hicpp-named-parameter,\
-cppcoreguidelines-avoid-non-const-global-variables,\
-concurrency-mt-unsafe,-altera-struct-pack-align,\
-altera-id-dependent-backward-branch,altera-unroll-loops,-misc-no-recursion,\
-altera-unroll-loops,-readability-function-cognitive-complexity,\
-bugprone-suspicious-include,-bugprone-easily-swappable-parameters\
	-header-filter=.* \
	-quiet -- \
	-std=c17 -I. $($LLVM_CONFIG --cflags) 2>&1 | tee /dev/stderr | grep "skull"
)" ]
