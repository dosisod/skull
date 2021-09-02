#include <stdbool.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/codegen/c/write.h"
#include "skull/codegen/llvm/write.h"
#include "skull/common/io.h"
#include "skull/common/malloc.h"

#include "skull/codegen/write.h"


bool write_file(const char *filename) {
	char *new_filename = NULL;
	bool err = false;

	if (BUILD_DATA.c_backend) {
		new_filename = gen_filename(filename, "c");
		err = write_file_c(new_filename);
	}
	else {
		if (BUILD_DATA.out_file && BUILD_DATA.preprocess) {
			new_filename = strdup(BUILD_DATA.out_file);
		}
		else if (BUILD_DATA.preprocess) {
			new_filename = strdup("-");
		}
		else {
			new_filename = gen_filename(filename, "ll");
		}

		err = write_file_llvm(new_filename);
	}

	free(new_filename);

	return err;
}
