#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/codegen/ast.h"
#include "skull/codegen/c/write.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/write.h"
#include "skull/codegen/shared.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/semantic/entry.h"

#include "skull/codegen/entry.h"

static char *gen_filename(const char *, const char *);
static bool codegen_pipeline(char *const);

/*
Setup codegen pipeline, including exporting of file.

Function takes ownership of `file_contents`.
*/
int init_codegen_pipeline(const char *filename, char *file_contents) {
	BUILD_DATA.filename = filename;
	setup_state_llvm();

	const bool failed = codegen_pipeline(file_contents);
	free(file_contents);

	if (failed) {
		free_state();
		return failed;
	}

	int err = 0;
	char *new_filename = NULL;

	if (BUILD_DATA.c_backend) {
		new_filename = gen_filename(filename, "c");
		err = write_file_c(new_filename);
	}
	else {
		new_filename = gen_filename(filename, "ll");
		err = write_file_llvm(new_filename);
	}

	free(new_filename);
	free_state_llvm();
	free_state();

	return err;
}

/*
Create Skull filename based on `filename` with extention `ext`.
*/
static char *gen_filename(const char *filename, const char *ext) {
	const size_t len = strlen(filename);
	const size_t ext_len = strlen(ext);
	char *const new_filename = Malloc(len + 3 + ext_len);

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

	new_filename[len + 1] = '.';
	strncpy(new_filename + len + 2, ext, ext_len + 1);

	return new_filename;
}

/*
Run "parse" > "validate" > "codegen" pipeline on file contents `str_`.

Return `true` if errors occurred.
*/
static bool codegen_pipeline(char *const str_) {
	char32_t *const str = mbstoc32s(str_);
	if (!str) return true;

	AstNode *const node = parse_ast_tree(str);
	if (!node) {
		free(str);
		return true;
	}

	bool err = true;

	if (validate_ast_tree(node)) err = gen_tree(node);

	free_ast_tree(node);
	free(str);

	return err;
}

