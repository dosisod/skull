#pragma once

#define SHIM "echo \'#include <stdint.h>\n" \
"#include <uchar.h>\n" \
"_Bool _strcmp(const char *a, const char *b) {\n" \
"	while (*a && *b) {\n" \
"		if (*a != *b) {\n" \
"			return 0;\n" \
"		}\n" \
"		a++;\n" \
"		b++;\n" \
"	}\n" \
"\n" \
"	return !*a && !*b;\n" \
"}\n" \
"#include <math.h>\n" \
"double (*_float_pow)(double, double) = pow;\n" \
"int64_t _int_pow(int64_t base, int64_t exp) {\n" \
"	int64_t result = base;\n" \
"	for (int64_t i = 1; i < exp; i++) result *= base;\n" \
"	return result;\n" \
"}\n" \
"int init(void) __asm__(\"%s\");\n" \
"int main(void) { return init(); }\n" \
"\' | cc \"%s\" -o \"%s\" %s -I/usr/include/skull -x c - -lm"
