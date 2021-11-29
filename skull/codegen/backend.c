#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/io.h"
#include "skull/build_data.h"

#include "skull/codegen/backend.h"

static char *get_new_filename(const char *, const char *);

bool run_backend(
	const Backend *backend,
	const AstNode *node,
	const char *filename
) {
	if (backend->setup) backend->setup();

	if (backend->gen_module) backend->gen_module(node);

	char *new_filename = get_new_filename(filename, backend->extension);
	const bool success = backend->write(new_filename);
	free(new_filename);

	if (backend->cleanup) backend->cleanup();

	return success;
}

static char *get_new_filename(const char *filename, const char *extension) {
	if (BUILD_DATA.out_file && (
		BUILD_DATA.preprocess || BUILD_DATA.c_backend
	)) {
		return strdup(BUILD_DATA.out_file);
	}
	if (BUILD_DATA.preprocess) {
		return strdup("-");
	}

	return gen_filename(filename, extension);
}
