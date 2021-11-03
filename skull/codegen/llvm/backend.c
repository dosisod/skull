#include "skull/codegen/write.h"
#include "skull/codegen/ast.h"
#include "skull/codegen/llvm/shared.h"

#include "skull/codegen/llvm/backend.h"

Backend llvm_backend = {
	.setup = setup_llvm_state,
	.write = write_file,
	.gen_module = gen_module,
	.cleanup = free_llvm_state
};
