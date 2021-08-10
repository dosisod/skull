#include <stdbool.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/codegen/c/write.h"
#include "skull/codegen/llvm/write.h"
#include "skull/common/malloc.h"

#include "skull/codegen/write.h"

static char *gen_filename(const char *, const char *);


bool write_file(const char *filename) {
	char *new_filename = NULL;
	bool err = false;

	if (BUILD_DATA.c_backend) {
		new_filename = gen_filename(filename, "c");
		err = write_file_c(new_filename);
	}
	else {
		new_filename = gen_filename(filename, "ll");
		err = write_file_llvm(new_filename);
	}

	free(new_filename);

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
