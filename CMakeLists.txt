cmake_minimum_required(VERSION 3.10.2)

project(skull VERSION 0.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)

set(EXTRA_COMPILER_FLAGS
	-Wall
	-Wextra
	-pedantic
	-Werror
	-Waggressive-loop-optimizations
	-Waggregate-return
	-Wbad-function-cast
	-Wbuiltin-macro-redefined
	-Wc99-c11-compat
	-Wcast-align
	-Wdate-time
	-Wdisabled-optimization
	-Wformat-nonliteral
	-Wformat-security
	-Wformat-signedness
	-Wformat-y2k
	-Winvalid-pch
	-Wlogical-op
	-Wmissing-include-dirs
	-Wnested-externs
	-Wpacked
	-Wredundant-decls
	-Wshadow
	-Wsuggest-attribute=const
	-Wsuggest-attribute=format
	-Wsuggest-attribute=noreturn
	-Wsuggest-attribute=pure
	-Wswitch-default
	-Wswitch-enum
	-Wsync-nand
	-Wtrampolines
	-Wundef
	-Wunsafe-loop-optimizations
	-Wunused
	-Wunused-macros
	-Wchkp
)

add_subdirectory(src)
add_subdirectory(test)
enable_testing()
add_test(NAME TestBuild COMMAND Test)