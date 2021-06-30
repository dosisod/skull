#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/io.h"

#include "skull/codegen/c/write.h"

static const char *data = \
"_Bool _strcmp(const char *a, const char *b) {\n"
"	while (*a && *b) {\n"
"		if (*a != *b) {\n"
"			return 0;\n"
"		}\n"
"		a++;\n"
"		b++;\n"
"	}\n"
"\n"
"	return !*a && !*b;\n"
"}\n"
"\n"
"#include <math.h>\n"
"double (*_float_pow)(double, double) = pow;\n"
"\n"
"#include <stdint.h>\n"
"int64_t _int_pow(int64_t base, int64_t exp) {\n"
"	int64_t result = base;\n"
"	for (int64_t i = 1; i < exp; i++) result *= base;\n"
"	return result;\n"
"}\n"
"\n"
"int main(void) {\n"
"	int x(void) __asm__(\"%s\"); return x();\n"
"}\n";



/*
Write c code to `filename`, return whether error occured.
*/
bool write_file_c(char *filename) {
	FILE *f = open_file(filename, false);

	const size_t len = strlen(filename);
	filename[len - 5] = '\0';

#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wformat-nonliteral"
#else
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
	fprintf(f, data, filename);
#ifdef __clang__
# pragma clang diagnostic pop
#else
# pragma GCC diagnostic pop
#endif

	return false;
}
