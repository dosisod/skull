#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Core.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/types/defs.h"

#include "skull/llvm/aliases.h"

#include "skull/llvm/var.h"

/*
Convert a Skull variable `var` into the LLVM IR equivalent.
*/
void var_to_llvm_ir(Variable *var, LLVMBuilderRef builder, LLVMContextRef ctx) {
	char *var_name = c32stombs(var->name);

	if (var->type == &TYPE_INT) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMInt64TypeInContext(ctx),
				var_name
			);
		}

		SkullInt num = 0;
		variable_read(&num, var);

		LLVMBuildStore(
			builder,
			LLVM_INT(ctx, num),
			var->alloca
		);
	}
	if (var->type == &TYPE_FLOAT) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMDoubleTypeInContext(ctx),
				var_name
			);
		}

		SkullFloat num = 0;
		variable_read(&num, var);

		LLVMBuildStore(
			builder,
			LLVM_FLOAT(ctx, num),
			var->alloca
		);
	}
	else if (var->type == &TYPE_BOOL) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMInt1TypeInContext(ctx),
				var_name
			);
		}

		bool val = false;
		variable_read(&val, var);

		LLVMBuildStore(
			builder,
			LLVM_BOOL(ctx, val),
			var->alloca
		);
	}
	else if (var->type == &TYPE_RUNE) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMInt32TypeInContext(ctx),
				var_name
			);
		}

		char32_t c = U'\0';
		variable_read(&c, var);

		LLVMBuildStore(
			builder,
			LLVM_RUNE(ctx, c),
			var->alloca
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
			llvm_arr[counter] = LLVM_RUNE(ctx, str[counter]);
			counter++;
		}

		LLVMTypeRef str_type = LLVMArrayType(
			LLVMInt32TypeInContext(ctx),
			(unsigned)len
		);

		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				str_type,
				var_name
			);
		}

		LLVMValueRef char_arr = LLVMConstArray(
			str_type,
			llvm_arr,
			(unsigned)len
		);
		free(llvm_arr);

		LLVMBuildStore(
			builder,
			char_arr,
			var->alloca
		);
	}

	free(var_name);
}