#include "skull/codegen/c/write.h"

#include "skull/codegen/c/backend.h"

Backend c_backend = {
	.write = write_file_c
};
