#include <stdlib.h>
#include <string.h>

#include "skull/common/io.h"
#include "skull/build_data.h"

#include "skull/codegen/shared.h"

char *get_new_filename(const char *filename, const char *extension) {
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
