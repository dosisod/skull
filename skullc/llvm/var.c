#include <stdlib.h>
#include <stdio.h>

#include <llvm-c/Core.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"

#include "skullc/llvm/aliases.h"

#include "skullc/llvm/var.h"

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
	if (var->type == &TYPE_FLOAT) {
		LLVMValueRef ir_var = LLVMBuildAlloca(
			builder,
			LLVMFP128TypeInContext(ctx),
			var_name
		);

		// suboptimal way of converting 64bit floats to llvm
		char32_t *tmp = var->type->to_string(var);
		char *var_as_str = c32stombs(tmp);
		free(tmp);

		LLVMBuildStore(
			builder,
			LLVM_FLOAT(ctx, var_as_str),
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
	else if (var->type == &TYPE_CHAR) {
		LLVMValueRef ir_var = LLVMBuildAlloca(
			builder,
			LLVMInt32TypeInContext(ctx),
			var_name
		);

		char32_t c = U'\0';
		variable_read(&c, var);

		LLVMBuildStore(
			builder,
			LLVM_CHAR(ctx, c),
			ir_var
		);
	}
	else if (var->type == &TYPE_STR) {
		char32_t *str = NULL;
		variable_read(&str, var);
		const size_t len = c32slen(str) + 1;

		LLVMValueRef *llvm_arr = malloc(sizeof(LLVMValueRef) * len);
		DIE_IF_MALLOC_FAILS(llvm_arr);

		size_t counter = 0;
		while (counter < len) {
			llvm_arr[counter] = LLVM_CHAR(ctx, str[counter]);
			counter++;
		}

		LLVMTypeRef str_type = LLVMArrayType(
			LLVMInt32TypeInContext(ctx),
			(unsigned)len
		);

		LLVMValueRef ir_var = LLVMBuildAlloca(
			builder,
			str_type,
			var_name
		);

		LLVMValueRef char_arr = LLVMConstArray(
			str_type,
			llvm_arr,
			(unsigned)len
		);
		free(llvm_arr);

		LLVMBuildStore(
			builder,
			char_arr,
			ir_var
		);
	}

	free(var_name);
}
