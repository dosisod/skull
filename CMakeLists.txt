cmake_minimum_required(VERSION 3.10.2)

project(skull VERSION 0.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)

set(EXTRA_COMPILER_FLAGS
	-Wall
	-Wextra
	-pedantic
	-Werror
	-Waggregate-return
	-Wbad-function-cast
	-Wbuiltin-macro-redefined
	-Wcast-align
	-Wdate-time
	-Wdisabled-optimization
	-Wformat-nonliteral
	-Wformat-security
	-Wformat-y2k
	-Winvalid-pch
	-Wmissing-include-dirs
	-Wnested-externs
	-Wpacked
	-Wredundant-decls
	-Wshadow
	-Wswitch-default
	-Wswitch-enum
	-Wundef
	-Wunused
	-Wunused-macros
	-Wconversion
	-Wfloat-equal
	-Wwrite-strings
)

if (CMAKE_C_COMPILER_ID MATCHES "GNU")
	#add additional flags that clang doesnt support
	set(EXTRA_COMPILER_FLAGS ${EXTRA_COMPILER_FLAGS}
		-Waggressive-loop-optimizations
		-Wc99-c11-compat
		-Wformat-signedness
		-Wlogical-op
		-Wsuggest-attribute=const
		-Wsuggest-attribute=format
		-Wsuggest-attribute=noreturn
		-Wsuggest-attribute=pure
		-Wsync-nand
		-Wtrampolines
		-Wunsafe-loop-optimizations
		-Wchkp
	)
else()
	#disable clang specific warning
	set(EXTRA_COMPILER_FLAGS ${EXTRA_COMPILER_FLAGS} -Wno-newline-eof)
endif()

add_subdirectory(src)
add_subdirectory(test)
enable_testing()
add_test(NAME TestBuild COMMAND Test)