#include "skull/codegen/c/core.h"
#include "skull/codegen/c/write.h"

#include "skull/codegen/c/backend.h"

Backend c_backend = {
	.write = write_file_c,
	.gen_module = gen_module_c
};
