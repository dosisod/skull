#include "skull/codegen/llvm/core.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/write.h"

#include "skull/codegen/llvm/backend.h"

Backend llvm_backend = {
	.setup = (void *(*)(void))setup_llvm_state,
	.write = (_Bool (*)(const char *, void *))write_llvm,
	.gen_module = (void (*)(const AstNode *, void *))gen_module,
	.cleanup = (void (*)(void *))free_llvm_state,
	.extension = "ll"
};
