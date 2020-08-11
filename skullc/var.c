#include <stdlib.h>

#include <llvm-c/Core.h>

#include "skull/common/str.h"

#include "skullc/llvm.h"

#include "skullc/var.h"

/*
Convert a Skull variable `var` into the LLVM IR equivalent.
*/
void var_to_llvm_ir(variable_t *var, LLVMBuilderRef builder, LLVMContextRef ctx) {
	if (var->type == &TYPE_INT) {
		char *var_name = c32stombs(var->name);

		LLVMValueRef ir_var = LLVMBuildAlloca(
			builder,
			LLVMInt64TypeInContext(ctx),
			var_name
		);
		free(var_name);

		int64_t num = 0;
		variable_read(&num, var);

		LLVMBuildStore(
			builder,
			LLVM_INT(ctx, num),
			ir_var
		);
	}
}
