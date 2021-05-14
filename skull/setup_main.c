#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/ast.h"
#include "skull/codegen/func.h"
#include "skull/codegen/setup_cleanup.h"
#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/common/io.h"
#include "skull/common/local.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/setup_main.h"

#define DIE(x) fprintf(stderr, "skull: %s\n", x); return 1

int build_file(char *);
char *gen_filename(const char *);

bool SKULL_TESTING = false;

/*
Actual `main` function, can be called by external programs.
*/
int setup_main(int argc, char *argv[]) {
	setup_locale();

	if (argc < 2) {
		DIE("unexpected number of parameters");
	}

	for (int i = 0; i < argc - 1 ; i++) {
		const int err = build_file(argv[i + 1]);
		if (err) return err;
	}

	return 0;
}

/*
Build a .ll file from a .sk file `filename`.
*/
int build_file(char *filename) {
	if (!strrstr(filename, ".sk")) {
		DIE("missing required \".sk\" extension, exiting");
	}

	if (strrstr(filename, ".sk") == filename || strrstr(filename, "/.sk")) {
		DIE("\".sk\" is not a valid name, exiting");
	}

	errno = 0;
	FILE *const f = fopen(filename, "re");
	if (!f) {
		if (errno == EACCES)
			fprintf(stderr, "skull: cannot open \"%s\", permission denied\n", filename);

		else if (errno == ENOENT)
			fprintf(stderr, "skull: \"%s\" was not found, exiting\n", filename);

		return 1;
	}

	char *const file_contents = read_file(f);
	if (!file_contents) {
		SKULL_STATE.filename = filename;

		FMT_WARN(WARN_FILE_EMPTY, {0});
		fclose(f);
		return 0;
	}
	fclose(f);

	const bool failed = generate_llvm(filename, file_contents);
	free(file_contents);

	if (failed) return failed;

	return write_file(gen_filename(filename));
}

/*
Create Skull filename based on `filename`.
*/
char *gen_filename(const char *filename) {
	const size_t len = strlen(filename);
	char *const new_filename = Malloc(len + 5);

	const char *const slash_pos = strrchr(filename, '/');
	if (!slash_pos) {
		new_filename[0] = '.';
		strncpy(new_filename + 1, filename, len + 1);
	}
	else {
		const long offset = slash_pos - filename;

		strncpy(new_filename, filename, len + 1);
		new_filename[offset + 1] = '.';
		strncpy(
			new_filename + offset + 2,
			slash_pos + 1, len - (size_t)offset
		);
	}
	strncpy(new_filename + len + 1, ".ll", 4);

	return new_filename;
}
