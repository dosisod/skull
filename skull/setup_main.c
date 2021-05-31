#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/ast.h"
#include "skull/codegen/c/write.h"
#include "skull/codegen/func.h"
#include "skull/codegen/llvm/write.h"
#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/common/io.h"
#include "skull/common/local.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/setup_main.h"

#define DIE(x) fprintf(stderr, "skull: %s\n", x); return 1

static int build_file(char *);
static char *gen_filename(const char *);
void setup_flags(void);

bool SKULL_TESTING = false;

/*
Actual `main` function, can be called by external programs.
*/
int setup_main(int argc, char *argv[]) {
	setup_locale();

	if (argc < 2) {
		DIE("unexpected number of parameters");
	}

	setup_flags();

	for (int i = 0; i < argc - 1 ; i++) {
		const int err = build_file(argv[i + 1]);
		if (err) return err;
	}

	return 0;
}

void setup_flags(void) {
	char *c_backend = getenv("C_BACKEND");
	SKULL_STATE.c_backend = c_backend ? *c_backend == '1' : false;
}

/*
Output compiled version of `filename` (file type depends on backend).
*/
static int build_file(char *filename) {
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
		SKULL_STATE.filename = filename;

		FMT_WARN(WARN_FILE_EMPTY, {0});
		fclose(f);
		return 0;
	}
	fclose(f);

	setup_state(&SKULL_STATE, filename);
	const bool failed = codegen_str(file_contents);
	free(file_contents);

	if (failed) {
		free_state(&SKULL_STATE);
		return failed;
	}

	char *llvm_filename = gen_filename(filename);
	int err = 0;

	if (SKULL_STATE.c_backend) {
		const size_t len = strlen(llvm_filename);
		llvm_filename[len - 2] = 'c';
		llvm_filename[len - 1] = '\0';
		err = write_file_c(llvm_filename);
	}
	else {
		err = write_file_llvm(llvm_filename);
	}

	free(llvm_filename);
	free_state(&SKULL_STATE);

	return err;
}

/*
Create Skull filename based on `filename`.
*/
static char *gen_filename(const char *filename) {
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
