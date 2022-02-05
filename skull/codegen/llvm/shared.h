#pragma once

#include "skull/codegen/llvm/fwd_decl.h"

typedef struct FunctionDeclaration FunctionDeclaration;

typedef struct {
	LLVMBuilderRef builder;
	LLVMContextRef ctx;
	LLVMModuleRef module;
	FunctionDeclaration *current_func;
	FunctionDeclaration *main_func;
	LLVMBasicBlockRef current_while_cond;
	LLVMBasicBlockRef current_while_end;
} SkullStateLLVM;

extern SkullStateLLVM SKULL_STATE_LLVM;

void free_llvm_state(void);
SkullStateLLVM *setup_llvm_state(void);
