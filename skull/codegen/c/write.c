#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/abi.h"
#include "skull/common/io.h"

#include "skull/codegen/c/write.h"

static const char *builtin_decls;


/*
Write c code to `filename`, return whether error occured.
*/
bool write_file_c(const char *filename) {
	char *c_filename = gen_filename(filename, "c");
	FILE *f = open_file(c_filename, false);
	free(c_filename);

	char *module_name = create_main_func_name(filename);

	fprintf(f, "%s\n", builtin_decls);

	fprintf(
		f,
		"static int init(void) __asm__(\"%s\");\n" \
		"static int init(void) { return 0; }\n" \
		"int main(void) { return init(); }\n",
		module_name
	);

	free(module_name);

	return false;
}

static const char *builtin_decls = \
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
