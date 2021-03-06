#pragma once

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/semantic/types.h"

LLVMTypeRef gen_llvm_type(Type);

LLVMTypeRef gen_llvm_func_type(Type, LLVMTypeRef *, unsigned);
