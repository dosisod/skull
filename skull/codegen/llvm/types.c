#include <stdbool.h>
#include <stddef.h>

#include <llvm-c/Core.h>

#include "skull/codegen/llvm/shared.h"

#include "skull/codegen/llvm/types.h"

/*
Generate the LLVM type for `type`.
*/
LLVMTypeRef type_to_llvm_type(const Type *type, const SkullStateLLVM *state) {
	if (type == &TYPE_BOOL) {
		return LLVMInt1TypeInContext(state->ctx);
	}
	if (type == &TYPE_INT) {
		return LLVMInt64TypeInContext(state->ctx);
	}
	if (type == &TYPE_FLOAT) {
		return LLVMDoubleTypeInContext(state->ctx);
	}
	if (type == &TYPE_RUNE) {
		return LLVMInt32TypeInContext(state->ctx);
	}
	if (type == &TYPE_STR) {
		return LLVMPointerType(
			LLVMInt8TypeInContext(state->ctx),
			0
		);
	}
	if (type == &TYPE_VOID) {
		return LLVMVoidTypeInContext(state->ctx);
	}
	if (is_reference(type)) {
		return LLVMPointerType(type_to_llvm_type(type->inner, state), 0);
	}

	return NULL;
}

LLVMTypeRef type_to_llvm_func_type(
	const Type *type,
	LLVMTypeRef *param_types,
	unsigned num_params,
	const SkullStateLLVM *state
) {
	return LLVMFunctionType(
		type_to_llvm_type(type, state),
		param_types,
		num_params,
		false
	);
}
