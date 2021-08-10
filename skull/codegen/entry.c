#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/codegen/ast.h"
#include "skull/codegen/c/write.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/write.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/semantic/entry.h"
#include "skull/semantic/shared.h"

#include "skull/codegen/entry.h"

static char *gen_filename(const char *, const char *);

/*
Run "parse" > "validate" > "codegen" pipeline on file contents `file_contents`.

Function takes ownership of `file_contents`.

Return `true` if errors occurred.
*/
int init_codegen_pipeline(const char *filename, char *file_contents) {
	BUILD_DATA.filename = filename;

	char32_t *const _file_contents = mbstoc32s(file_contents);
	if (!_file_contents) {
		free(file_contents);
		return true;
	}

	AstNode *node = parse_ast_tree(_file_contents);
	if (!node) {
		free(_file_contents);
		free(file_contents);
		return true;
	}

	bool err = true;

	if (validate_ast_tree(node)) {
		setup_llvm_state();

		if (BUILD_DATA.debug) {
			setup_debug_info(filename, SKULL_STATE_LLVM.module);
		}

		err = gen_tree(node);
	}

	free_ast_tree(node);
	free(_file_contents);
	free(file_contents);

	if (err) {
		free_llvm_state();
		free_semantic_state();
		return err;
	}

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
	free_llvm_state();
	free_semantic_state();

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
