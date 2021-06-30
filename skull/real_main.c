#include <stdbool.h>
#include <stdio.h>

#include "skull/build_data.h"
#include "skull/codegen/entry.h"
#include "skull/common/errors.h"
#include "skull/common/io.h"
#include "skull/common/local.h"
#include "skull/common/str.h"

#include "skull/real_main.h"

#define DIE(x) fprintf(stderr, "skull: %s\n", x); return 1

static int compile_file(char *);
static void setup_flags(void);

bool SKULL_TESTING = false;

/*
Actual `main` function, can be called by external programs.
*/
int real_main(int argc, char *argv[]) {
	setup_locale();

	if (argc < 2) {
		DIE("unexpected number of parameters");
	}

	setup_flags();

	for (int i = 0; i < argc - 1 ; i++) {
		const int err = compile_file(argv[i + 1]);
		if (err) return err;
	}

	return 0;
}

static void setup_flags(void) {
	char *c_backend = getenv("C_BACKEND");
	BUILD_DATA.c_backend = c_backend ? *c_backend == '1' : false;
}

/*
Output compiled version of `filename` (file type depends on backend).
*/
static int compile_file(char *filename) {
	if (!strrstr(filename, ".sk")) {
		DIE("missing required \".sk\" extension, exiting");
	}

	if (strrstr(filename, ".sk") == filename || strrstr(filename, "/.sk")) {
		DIE("\".sk\" is not a valid name, exiting");
	}

	FILE *const f = open_file(filename, true);
	if (!f) return 1;

	char *const file_contents = read_file(f);
	if (!file_contents) {
		BUILD_DATA.filename = filename;

		FMT_WARN(WARN_FILE_EMPTY, {0});
		fclose(f);
		return 0;
	}
	fclose(f);

	return init_codegen_pipeline(filename, file_contents);
}
