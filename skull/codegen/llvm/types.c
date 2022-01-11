#include <stdbool.h>
#include <stddef.h>

#include <llvm-c/Core.h>

#include "skull/codegen/llvm/shared.h"

#include "skull/codegen/llvm/types.h"

/*
Generate the LLVM type for `type`.
*/
LLVMTypeRef type_to_llvm_type(const Type *type) {
	if (type == &TYPE_BOOL) {
		return LLVMInt1TypeInContext(SKULL_STATE_LLVM.ctx);
	}
	if (type == &TYPE_INT) {
		return LLVMInt64TypeInContext(SKULL_STATE_LLVM.ctx);
	}
	if (type == &TYPE_FLOAT) {
		return LLVMDoubleTypeInContext(SKULL_STATE_LLVM.ctx);
	}
	if (type == &TYPE_RUNE) {
		return LLVMInt32TypeInContext(SKULL_STATE_LLVM.ctx);
	}
	if (type == &TYPE_STR) {
		return LLVMPointerType(
			LLVMInt8TypeInContext(SKULL_STATE_LLVM.ctx),
			0
		);
	}
	if (type == &TYPE_VOID) {
		return LLVMVoidTypeInContext(SKULL_STATE_LLVM.ctx);
	}
	if (is_reference(type)) {
		return LLVMPointerType(type_to_llvm_type(type->inner), 0);
	}

	return NULL;
}

LLVMTypeRef type_to_llvm_func_type(
	const Type *type,
	LLVMTypeRef *param_types,
	unsigned num_params
) {
	return LLVMFunctionType(
		type_to_llvm_type(type),
		param_types,
		num_params,
		false
	);
}
