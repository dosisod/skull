#include "skull/codegen/c/core.h"
#include "skull/codegen/c/shared.h"
#include "skull/codegen/c/write.h"

#include "skull/codegen/c/backend.h"

Backend c_backend = {
	.setup = (void *(*)(SemanticState *))setup_c_state,
	.write = (_Bool (*)(const char *, void *))write_file_c,
	.gen_module = (void (*)(const AstNode *, void *))gen_module_c,
	.extension = "c",
	.cleanup = (void (*)(void *))free_c_state
};
