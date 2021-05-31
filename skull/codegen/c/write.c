#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/ast.h"
#include "skull/codegen/func.h"
#include "skull/codegen/shared.h"

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
"}\n";



/*
Write c code to `filename`, return whether error occured.
*/
bool write_file_c(char *filename) {
	// TODO(dosisod): make this a function
	errno = 0;
	FILE *const f = fopen(filename, "we");
	if (!f) {
		if (errno == EACCES)
			fprintf(
				stderr,
				"skull: cannot open \"%s\", permission denied\n",
				filename
			);

		else if (errno == ENOENT)
			fprintf(
				stderr,
				"skull: \"%s\" was not found, exiting\n",
				filename
			);

		return 1;
	}

	fwrite(data, 1, strlen(data), f);

	return false;
}
