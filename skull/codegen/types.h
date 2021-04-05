#pragma once

#include "skull/compiler/types/types.h"

#include <llvm-c/Core.h>

LLVMTypeRef gen_llvm_type(const Type *const);

LLVMTypeRef gen_llvm_bool_type(void);
LLVMTypeRef gen_llvm_int_type(void);
LLVMTypeRef gen_llvm_float_type(void);
LLVMTypeRef gen_llvm_rune_type(void);
LLVMTypeRef gen_llvm_str_type(void);
