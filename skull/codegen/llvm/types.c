#include <stdbool.h>
#include <stddef.h>

#include <llvm-c/Core.h>

#include "skull/codegen/shared.h"

#include "skull/codegen/llvm/types.h"

/*
Generate the LLVM type for `type`.
*/
LLVMTypeRef gen_llvm_type(Type type) {
	if (type == TYPE_BOOL) {
		return LLVMInt1TypeInContext(SKULL_STATE.ctx);
	}
	if (type == TYPE_INT) {
		return LLVMInt64TypeInContext(SKULL_STATE.ctx);
	}
	if (type == TYPE_FLOAT) {
		return LLVMDoubleTypeInContext(SKULL_STATE.ctx);
	}
	if (type == TYPE_RUNE) {
		return LLVMInt32TypeInContext(SKULL_STATE.ctx);
	}
	if (type == TYPE_STR) {
		return LLVMPointerType(
			LLVMInt8TypeInContext(SKULL_STATE.ctx),
			0
		);
	}
	if (type == TYPE_VOID) {
		return LLVMVoidTypeInContext(SKULL_STATE.ctx);
	}

	return NULL;
}

LLVMTypeRef gen_llvm_func_type(
	Type type,
	LLVMTypeRef *param_types,
	unsigned num_params
) {
	return LLVMFunctionType(
		gen_llvm_type(type),
		param_types,
		num_params,
		false
	);
}
