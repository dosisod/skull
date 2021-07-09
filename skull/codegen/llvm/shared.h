#pragma once

#include "skull/codegen/llvm/fwd_decl.h"

typedef struct FunctionDeclaration FunctionDeclaration;

typedef struct {
	LLVMBuilderRef builder;
	LLVMContextRef ctx;
	LLVMModuleRef module;
	FunctionDeclaration *current_func;
	FunctionDeclaration *main_func;
} SkullStateLLVM;

extern SkullStateLLVM SKULL_STATE_LLVM;

void free_state_llvm(void);
void setup_state_llvm(void);

char *create_main_func_name(const char *);
