#include <stdlib.h>

#include <llvm-c/Core.h>

#include "skull/common/str.h"

#include "skullc/llvm.h"

#include "skullc/var.h"

/*
Convert a Skull variable `var` into the LLVM IR equivalent.
*/
void var_to_llvm_ir(variable_t *var, LLVMBuilderRef builder, LLVMContextRef ctx) {
	char *var_name = c32stombs(var->name);

	if (var->type == &TYPE_INT) {
		LLVMValueRef ir_var = LLVMBuildAlloca(
			builder,
			LLVMInt64TypeInContext(ctx),
			var_name
		);

		int64_t num = 0;
		variable_read(&num, var);

		LLVMBuildStore(
			builder,
			LLVM_INT(ctx, num),
			ir_var
		);
	}
	else if (var->type == &TYPE_BOOL) {
		LLVMValueRef ir_var = LLVMBuildAlloca(
			builder,
			LLVMInt1TypeInContext(ctx),
			var_name
		);

		bool val = false;
		variable_read(&val, var);

		LLVMBuildStore(
			builder,
			LLVM_BOOL(ctx, val),
			ir_var
		);
	}

	free(var_name);
}
