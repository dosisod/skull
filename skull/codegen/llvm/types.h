#pragma once

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/semantic/types.h"

LLVMTypeRef type_to_llvm_type(const Type *, const SkullStateLLVM *);

LLVMTypeRef type_to_llvm_func_type(
	const Type *,
	LLVMTypeRef *,
	unsigned,
	const SkullStateLLVM *
);
