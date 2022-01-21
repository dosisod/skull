#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/codegen/abi.h"
#include "skull/codegen/c/shared.h"
#include "skull/codegen/shared.h"
#include "skull/common/io.h"

#include "skull/codegen/c/write.h"

static const char *_strcmp; // NOLINT
static const char *_float_pow; // NOLINT
static const char *_int_pow; // NOLINT
static const char *headers;

static void print_builtins(FILE *);
static void print_headers(FILE *);
static void print_globals(FILE *);
static void print_main_shim(FILE *, const char *);

/*
Write C code for given `filename` to new file, with ".sk" replaced with ".c".
*/
bool write_file_c(const char *filename) {
	char *out_filename = get_new_filename(filename, "c");

	FILE *f = strcmp(out_filename, "-") == 0 ?
		stdout :
		open_file(out_filename, false);

	free(out_filename);

	print_headers(f);
	print_builtins(f);
	print_globals(f);

	char *module_name = create_main_func_name(BUILD_DATA.filename);
	print_main_shim(f, module_name);
	free(module_name);

	return false;
}

static void print_headers(FILE *f) {
	fprintf(f, "%s\n", headers);
}

static void print_globals(FILE *f) {
	if (SKULL_STATE_C.globals) fprintf(f, "%s\n\n", SKULL_STATE_C.globals);
}

static void print_builtins(FILE *f) {
	if (SKULL_STATE_C.called_strcmp) fprintf(f, "%s\n", _strcmp);
	if (SKULL_STATE_C.called_int_pow) fprintf(f, "%s\n", _int_pow);
	if (SKULL_STATE_C.called_float_pow) fprintf(f, "%s\n", _float_pow);
}

static void print_main_shim(FILE *f, const char *module_name) {
	fprintf(
		f,
		"static int init(void) __asm__(\"%s\");\n" \
		"static int init(void) {\n%s\n\treturn 0;\n}\n" \
		"int main(void) { return init(); }\n",
		module_name,
		SKULL_STATE_C.tree ? SKULL_STATE_C.tree : ""
	);
}


static const char *headers = \
"#include <stdint.h>\n" \
"#include <uchar.h>\n";

static const char *_strcmp = \
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
"}\n";


static const char *_float_pow = \
"#include <math.h>\n"
"double (*_float_pow)(double, double) = pow;\n";

static const char *_int_pow = \
"int64_t _int_pow(int64_t base, int64_t exp) {\n"
"	int64_t result = base;\n"
"	for (int64_t i = 1; i < exp; i++) result *= base;\n"
"	return result;\n"
"}\n";
