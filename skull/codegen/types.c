#include <stddef.h>

#include "skull/codegen/shared.h"
#include "skull/eval/types/types.h"

#include "skull/codegen/types.h"

LLVMTypeRef gen_llvm_type(const Type *const type) {
	if (type == &TYPE_BOOL) {
		return gen_llvm_bool_type();
	}
	if (type == &TYPE_INT) {
		return gen_llvm_int_type();
	}
	if (type == &TYPE_FLOAT) {
		return gen_llvm_float_type();
	}
	if (type == &TYPE_RUNE) {
		return gen_llvm_rune_type();
	}
	if (type == &TYPE_STR) {
		return gen_llvm_str_type();
	}

	return NULL;
}

LLVMTypeRef gen_llvm_bool_type(void) {
	return LLVMInt1TypeInContext(SKULL_STATE.ctx);
}

LLVMTypeRef gen_llvm_int_type(void) {
	return LLVMInt64TypeInContext(SKULL_STATE.ctx);
}

LLVMTypeRef gen_llvm_float_type(void) {
	return LLVMDoubleTypeInContext(SKULL_STATE.ctx);
}

LLVMTypeRef gen_llvm_rune_type(void) {
	return LLVMInt32TypeInContext(SKULL_STATE.ctx);
}

LLVMTypeRef gen_llvm_str_type(void) {
	return LLVMPointerType(
		LLVMInt8TypeInContext(SKULL_STATE.ctx),
		0
	);
}
